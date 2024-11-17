{ pkgs, lib, config, inputs, ... }:

{
  languages.python = {
    enable = true;
    version = "3.12";

    venv.enable = true;
    venv.requirements = ./requirements.txt;
  };

  packages = [
    pkgs.age
  ];
}
