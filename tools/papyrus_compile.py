import argparse
import pathlib
import shutil
import subprocess
import zipfile


def extract_vanilla_scripts(
        game_dir: pathlib.Path,
        out_dir: pathlib.Path) -> None:
    out_dir.mkdir(exist_ok=True)
    archive = zipfile.ZipFile(game_dir / "Data" / "Scripts.zip", mode="r")
    scripts = [x for x in archive.namelist() if x.lower().startswith("source/scripts")]
    for script in scripts:
        with archive.open(script, mode="r") as f:
            dst = out_dir / pathlib.Path(script).name
            dst.write_bytes(f.read())


def compile(
        game_dir: pathlib.Path,
        build_dir: pathlib.Path,
        source_dir: pathlib.Path,
        skyui_dir: pathlib.Path,
        copy_build: bool) -> None:
    vanilla_dir = build_dir / "vanilla"
    out_dir = build_dir / "artifacts"

    extract_vanilla_scripts(game_dir, vanilla_dir)
    subprocess.run([
        game_dir / "Papyrus Compiler" / "PapyrusCompiler.exe",
        source_dir,
        "-import={};{};{}".format(vanilla_dir, source_dir, skyui_dir),
        "-output={}".format(out_dir),
        "-flags={}".format(vanilla_dir / "TESV_Papyrus_Flags.flg"),
        "-all",
    ])

    if copy_build:
        for file in out_dir.iterdir():
            shutil.copy(file, game_dir / "Data" / "Scripts" / file.name)


def parse_arguments():
    parser = argparse.ArgumentParser(description="compile papyrus source scripts")
    parser.add_argument("--copy-build", action="store_true", help="copy the compiled artifacts into the game directory")
    parser.add_argument("--game-dir", type=pathlib.Path, required=True, help="the game directory")
    parser.add_argument("--skyui-dir", type=pathlib.Path, help="the source directory for files to be compiled")
    parser.add_argument("--source-dir", type=pathlib.Path, help="the source directory for files to be compiled")
    return parser.parse_args()


def main():
    args = parse_arguments()
    compile(
        game_dir=args.game_dir,
        build_dir=pathlib.Path.cwd(),
        source_dir=args.source_dir,
        skyui_dir=args.skyui_dir,
        copy_build=args.copy_build)


if __name__ == "__main__":
    main()
