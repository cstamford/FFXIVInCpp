
using Lumina;
using System.Diagnostics;
using System.Reflection;

const string FFXIV_PATH = @"C:\Program Files (x86)\SquareEnix\FINAL FANTASY XIV - A Realm Reborn\game\sqpack";
const string API_PATH = @"D:\_ff14\FFXIVInCpp\src\Offsets\Generated";

GameData lumina = new(FFXIV_PATH);

var data_jobs = lumina.GetExcelSheet<Lumina.Excel.GeneratedSheets.ClassJob>();
var data_actions = lumina.GetExcelSheet<Lumina.Excel.GeneratedSheets.Action>();

List<Action> actions = new();

foreach (var row in data_actions)
{
    actions.Add(new Action
    {
        name = row.Name,
        id = row.RowId,
        level = row.ClassJobLevel,
        primary_cd_group = row.CooldownGroup,
        secondary_cd_group = row.AdditionalCooldownGroup,
        targets_self = row.CanTargetSelf,
        targets_friendly = row.CanTargetFriendly,
        targets_hostile = row.CanTargetHostile,
        range = (byte)row.Range,
        radius = (byte)row.EffectRange,
        max_charges = row.MaxCharges
    });
}

List<Job> jobs = new();

foreach (var row in data_jobs)
{
    jobs.Add(new Job
    {
        name = row.Name,
        abbrev = row.Abbreviation,
        index = row.RowId
    });
}

List<(string, List<string>)> output_jobs = JobEnums(jobs, actions);
List<(string, List<string>)> output_actions = Transpile(actions);

foreach (var (file_name, file_contents) in output_jobs.Concat(output_actions))
{
    Directory.CreateDirectory(API_PATH);
    File.WriteAllLines(Path.Combine(API_PATH, file_name), file_contents);
}

List<(string, List<string>)> Transpile<T>(List<T> data)
{
    string name = typeof(T).Name;

    List<string> header = new()
    {
        "#pragma once",
        "",
        "#include <cstdint>",
        "",
        $"struct {name}",
        "{",
        typeof(T).GetFields().Select(t => $"    {CppType(t.FieldType)} {t.Name};").Aggregate((a, b) => (a + "\n" + b)),
        "};",
        "",
        $"extern {name} {name.ToUpper()}S[{data.Count}];"
    };

    List<string> source = new()
    {
        $"#include \"{name}s.hpp\"",
        "",
        $"{name} {name.ToUpper()}S[{data.Count}] =",
        "{",
    };

    foreach (T entry in data)
    {
        source.Add($"    {{ {typeof(T).GetFields().Select(f => CppValue(f, entry)).Aggregate((a, b) => (a + ", " + b))} }},");
    }

    source.Add("};");

    return new()
    {
        { ($"{name}s.hpp", header) },
        { ($"{name}s.cpp", source) }
    };
}

List<(string, List<string>)> JobEnums(List<Job> jobs, List<Action> actions)
{
    List<string> header = new()
    {
        "#pragma once",
        "",
        "enum class Job",
        "{",
        jobs.Select((j, idx) => $"    {j.abbrev} = {idx}").Aggregate((a, b) => a + ",\n" + b),
        "};",
        ""
    };

    return new()
    {
        { ("Jobs.hpp", header) },
    };
}

string CppType(Type type)
{
    switch (type)
    {
        case Type str when str == typeof(string): return "const char*";
        case Type boolean when boolean == typeof(bool): return "bool";
        case Type uint8 when uint8 == typeof(byte): return "uint8_t";
        case Type uint32 when uint32 == typeof(uint): return "uint32_t";
    }

    Debug.Assert(false, "Unknown type - add it!");
    return type.Name;
}

string CppValue(FieldInfo field, object obj)
{
    switch (field.FieldType)
    {
        case Type str when str == typeof(string): return $"\"{field.GetValue(obj)}\"";
        case Type boolean when boolean == typeof(bool): return (bool)field.GetValue(obj) ? "true" : "false";
    }

    return field.GetValue(obj).ToString();
}

public class Job
{
    public string name;
    public string abbrev;
    public uint index;
}

public class Action
{
    public string name;
    public uint id;
    public byte level;
    public byte primary_cd_group;
    public byte secondary_cd_group;
    public bool targets_self;
    public bool targets_friendly;
    public bool targets_hostile;
    public byte range;
    public byte radius;
    public byte max_charges;
}


