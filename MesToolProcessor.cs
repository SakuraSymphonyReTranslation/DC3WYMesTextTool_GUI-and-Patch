using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;

namespace DC3WYMesTextTool
{
    public class MesToolProcessor
    {
        private static readonly Encoding ShiftJIS;
        private static readonly JsonSerializerOptions JsonOptions = new()
        {
            WriteIndented = true,
            Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping
        };

        static MesToolProcessor()
        {
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
            ShiftJIS = Encoding.GetEncoding(932);
        }

        public event Action<string>? Log;

        private void LogMessage(string message)
        {
            Log?.Invoke(message);
        }

        private void LogError(string message)
        {
            Log?.Invoke($"Error: {message}");
        }

        public int ExportMesToJson(string inputPath, string outputPath)
        {
            LogMessage($"Exporting {Path.GetFileName(inputPath)}... ");
            
            try
            {
                var script = new MesScript();
                script.Load(inputPath);
                
                var entries = script.ExtractText(ShiftJIS);
                
                if (entries.Count < 5)
                {
                    var patternEntries = script.ExtractTextPatternBased(ShiftJIS);
                    if (patternEntries.Count > entries.Count)
                        entries = patternEntries;
                }
                
                // When exporting a translated MES from AdvData\MES, prefer the
                // matching Japanese file in AdvData\MES asli Jepang as the role
                // map.  Translation work can change quote glyphs or leave a
                // damaged opcode in an old file; the original script remains the
                // authoritative nameplate/dialogue layout.
                var roleEntries = TryLoadJapaneseReference(inputPath, entries.Count);
                var processedEntries = ProcessEntries(entries, roleEntries, script.Info);
                
                var outputList = new List<object>();
                foreach (var entry in processedEntries)
                {
                    if (!string.IsNullOrEmpty(entry.Name))
                    {
                        outputList.Add(new { name = entry.Name, message = entry.Message });
                    }
                    else
                    {
                        outputList.Add(new { message = entry.Message });
                    }
                }
                
                string json = JsonSerializer.Serialize(outputList, JsonOptions);
                File.WriteAllText(outputPath, json, Encoding.UTF8);
                
                LogMessage($"OK ({outputList.Count} entries)");
                return 0;
            }
            catch (Exception ex)
            {
                LogError($"Failed to export {inputPath}: {ex.Message}");
                return 1;
            }
        }

        /// <summary>
        /// Convert the MES slot stream into the editor-friendly JSON shape.
        ///
        /// In DC3WY a speaker name and its dialogue are two consecutive text
        /// slots.  The nameplate uses encrypted-string opcode 0x61 and the text
        /// uses opcode 0x62.  The original exporter emitted both slots as
        /// { message: ... }, which made the relationship invisible to
        /// translators. Opcode roles also cover thoughts and translated text
        /// whose opening quotes differ from the Japanese original.
        ///
        /// The importer expands { name, message } back to the same two slots,
        /// so this is a presentation-only transformation and does not alter the
        /// MES instruction stream.
        /// </summary>
        private static List<TextEntry> ProcessEntries(List<TextEntry> entries, List<TextEntry>? roleEntries, ScriptInfo info)
        {
            var processed = new List<TextEntry>(entries.Count);
            byte nameOpcode = (byte)(info.EncStr.Begin + 1);
            byte messageOpcode = (byte)(info.EncStr.Begin + 2);

            for (int i = 0; i < entries.Count; i++)
            {
                var current = entries[i];
                byte roleOpcode = roleEntries != null ? roleEntries[i].Opcode : current.Opcode;
                byte nextRoleOpcode = i + 1 < entries.Count
                    ? (roleEntries != null ? roleEntries[i + 1].Opcode : entries[i + 1].Opcode)
                    : (byte)0;

                if (roleOpcode == nameOpcode &&
                    i + 1 < entries.Count &&
                    nextRoleOpcode == messageOpcode)
                {
                    processed.Add(new TextEntry
                    {
                        Name = current.Message,
                        Message = entries[i + 1].Message
                    });
                    i++;
                    continue;
                }

                processed.Add(new TextEntry
                {
                    Message = current.Message
                });
            }

            return processed;
        }

        private static List<TextEntry>? TryLoadJapaneseReference(string inputPath, int expectedCount)
        {
            string? inputDirectory = Path.GetDirectoryName(Path.GetFullPath(inputPath));
            if (string.IsNullOrEmpty(inputDirectory))
                return null;

            var parent = Directory.GetParent(inputDirectory);
            if (parent == null || !parent.Name.Equals("AdvData", StringComparison.OrdinalIgnoreCase))
                return null;

            string referencePath = Path.Combine(parent.FullName, "MES asli Jepang", Path.GetFileName(inputPath));
            if (!File.Exists(referencePath) ||
                Path.GetFullPath(referencePath).Equals(Path.GetFullPath(inputPath), StringComparison.OrdinalIgnoreCase))
                return null;

            try
            {
                var reference = new MesScript();
                reference.Load(referencePath);
                var entries = reference.ExtractText(ShiftJIS);
                return entries.Count == expectedCount ? entries : null;
            }
            catch
            {
                // A missing/malformed reference must not prevent ordinary
                // single-file export; fall back to the input's own opcodes.
                return null;
            }
        }

        private static bool ShouldWrapStoryScript(string path)
        {
            string baseName = Path.GetFileNameWithoutExtension(path);
            return baseName.StartsWith("wy_", StringComparison.OrdinalIgnoreCase);
        }

        public int ImportJsonToMes(string jsonPath, string originalMesPath, string outputPath, int wrapWidth = TextFormatter.DefaultWidth)
        {
            LogMessage($"Importing {Path.GetFileName(jsonPath)}... ");
            
            try
            {
                TextFormatter.ValidateWidth(wrapWidth);
                string json = File.ReadAllText(jsonPath, Encoding.UTF8);
                var entries = JsonSerializer.Deserialize<List<TextEntry>>(json) 
                    ?? new List<TextEntry>();
                
                var expandedEntries = new List<TextEntry>();
                var jsonIndices = new List<int>();
                for (int i = 0; i < entries.Count; i++)
                {
                    var entry = entries[i] ?? throw new InvalidDataException($"JSON entry {i + 1} is null.");
                    if (!string.IsNullOrEmpty(entry.Name))
                    {
                        expandedEntries.Add(new TextEntry { Message = entry.Name });
                        jsonIndices.Add(i + 1);
                    }
                    expandedEntries.Add(new TextEntry { Message = entry.Message });
                    jsonIndices.Add(i + 1);
                }
                
                var script = new MesScript();
                script.Load(originalMesPath);
                
                TextFormatter? formatter = ShouldWrapStoryScript(originalMesPath)
                    ? new TextFormatter
                    {
                        MaxLineLength = wrapWidth
                    }
                    : null;
                var warnings = new List<LayoutWarning>();
                byte[] newMes = script.ImportText(expandedEntries, ShiftJIS, formatter, warnings.Add);
                string fullOutputPath = Path.GetFullPath(outputPath);
                Directory.CreateDirectory(Path.GetDirectoryName(fullOutputPath)!);
                string reportPath = fullOutputPath + ".layout.json";
                var report = new
                {
                    source = Path.GetFullPath(jsonPath),
                    wrapWidth,
                    recommendedLines = TextFormatter.RecommendedLines,
                    warningCount = warnings.Count,
                    warnings = warnings.Select(w => new
                    {
                        jsonEntry = jsonIndices[w.MesSlot - 1],
                        mesSlot = w.MesSlot,
                        reason = w.Reason,
                        message = w.Message
                    })
                };
                // Always refresh the report, including zero warnings, so an old
                // report cannot be mistaken for the result of this import.
                File.WriteAllText(reportPath, JsonSerializer.Serialize(report, JsonOptions), Encoding.UTF8);
                File.WriteAllBytes(outputPath, newMes);
                
                LogMessage($"OK ({entries.Count} entries -> {expandedEntries.Count} expanded)");
                if (warnings.Count > 0)
                    LogMessage($"Layout warnings: {warnings.Count}. Text was preserved. Review {reportPath}");
                return 0;
            }
            catch (Exception ex)
            {
                LogError($"Failed to import {jsonPath}: {ex.Message}");
                return 1;
            }
        }

        public int StructureJsonFile(string jsonPath, string originalMesPath, string outputPath)
        {
            LogMessage($"Structuring {Path.GetFileName(jsonPath)}... ");

            try
            {
                string json = File.ReadAllText(jsonPath, Encoding.UTF8);
                var jsonEntries = JsonSerializer.Deserialize<List<TextEntry>>(json)
                    ?? new List<TextEntry>();

                var translatedSlots = ExpandJsonEntries(jsonEntries);

                var original = new MesScript();
                original.Load(originalMesPath);
                var roleEntries = original.ExtractText(ShiftJIS);

                if (translatedSlots.Count != roleEntries.Count)
                {
                    throw new InvalidDataException(
                        $"JSON expands to {translatedSlots.Count} text slots, but " +
                        $"the Japanese MES contains {roleEntries.Count}.");
                }

                var structured = ProcessEntries(translatedSlots, roleEntries, original.Info);
                string outputJson = JsonSerializer.Serialize(structured, JsonOptions);

                string fullOutputPath = Path.GetFullPath(outputPath);
                Directory.CreateDirectory(Path.GetDirectoryName(fullOutputPath)!);
                string tempPath = fullOutputPath + ".tmp";
                File.WriteAllText(tempPath, outputJson, Encoding.UTF8);
                File.Move(tempPath, fullOutputPath, true);

                LogMessage($"OK ({jsonEntries.Count} entries -> {structured.Count} structured, {translatedSlots.Count} slots preserved)");
                return 0;
            }
            catch (Exception ex)
            {
                LogError($"Failed to structure {jsonPath}: {ex.Message}");
                return 1;
            }
        }

        public int BatchStructureJson(string jsonFolder, string originalMesFolder, string outputFolder)
        {
            if (!Directory.Exists(jsonFolder))
            {
                LogError($"JSON folder not found: {jsonFolder}");
                return 1;
            }

            if (!Directory.Exists(originalMesFolder))
            {
                LogError($"Japanese MES folder not found: {originalMesFolder}");
                return 1;
            }

            Directory.CreateDirectory(outputFolder);
            var jsonFiles = Directory.GetFiles(jsonFolder, "*.json");
            int success = 0;
            int failed = 0;

            foreach (var jsonFile in jsonFiles)
            {
                string baseName = Path.GetFileNameWithoutExtension(jsonFile);
                string mesFile = Path.Combine(originalMesFolder, baseName + ".mes");
                string outputPath = Path.Combine(outputFolder, Path.GetFileName(jsonFile));

                if (!File.Exists(mesFile))
                {
                    LogError($"Japanese MES not found for {baseName}");
                    failed++;
                    continue;
                }

                if (StructureJsonFile(jsonFile, mesFile, outputPath) == 0)
                    success++;
                else
                    failed++;
            }

            LogMessage($"Batch Structure Completed: {success} success, {failed} failed");
            return failed > 0 ? 1 : 0;
        }

        private static List<TextEntry> ExpandJsonEntries(IEnumerable<TextEntry> entries)
        {
            var expanded = new List<TextEntry>();
            foreach (var entry in entries)
            {
                if (!string.IsNullOrEmpty(entry.Name))
                    expanded.Add(new TextEntry { Message = entry.Name });

                expanded.Add(new TextEntry { Message = entry.Message });
            }

            return expanded;
        }


        public int BatchExport(string inputFolder, string outputFolder)
        {
            if (!Directory.Exists(inputFolder))
            {
                LogError($"Folder not found: {inputFolder}");
                return 1;
            }

            Directory.CreateDirectory(outputFolder);
            var mesFiles = Directory.GetFiles(inputFolder, "*.mes");
            LogMessage($"Found {mesFiles.Length} MES files in {inputFolder}");

            int success = 0;
            int failed = 0;

            foreach (var mesFile in mesFiles)
            {
                string outputPath = Path.Combine(outputFolder, Path.GetFileNameWithoutExtension(mesFile) + ".json");
                if (ExportMesToJson(mesFile, outputPath) == 0)
                    success++;
                else
                    failed++;
            }
            
            LogMessage($"Batch Export Completed: {success} success, {failed} failed");
            return failed > 0 ? 1 : 0;
        }

        public int BatchImport(string jsonFolder, string mesFolder, string outputFolder, int wrapWidth = TextFormatter.DefaultWidth)
        {
            if (!Directory.Exists(jsonFolder))
            {
                LogError($"JSON folder not found: {jsonFolder}");
                return 1;
            }
            
            if (!Directory.Exists(mesFolder))
            {
                LogError($"MES folder not found: {mesFolder}");
                return 1;
            }

            Directory.CreateDirectory(outputFolder);
            var jsonFiles = Directory.GetFiles(jsonFolder, "*.json")
                .Where(path => !path.EndsWith(".mes.layout.json", StringComparison.OrdinalIgnoreCase)).ToArray();
            LogMessage($"Found {jsonFiles.Length} JSON files (wrap width: {wrapWidth})");

            int success = 0;
            int failed = 0;

            foreach (var jsonFile in jsonFiles)
            {
                string baseName = Path.GetFileNameWithoutExtension(jsonFile);
                string mesFile = Path.Combine(mesFolder, baseName + ".mes");
                string outputPath = Path.Combine(outputFolder, baseName + ".mes");
                
                if (!File.Exists(mesFile))
                {
                    LogMessage($"Skipping {baseName} - original MES not found");
                    continue;
                }
                
                if (ImportJsonToMes(jsonFile, mesFile, outputPath, wrapWidth) == 0)
                    success++;
                else
                    failed++;
            }

            LogMessage($"Batch Import Completed: {success} success, {failed} failed");
            return failed > 0 ? 1 : 0;
        }
    }
}
