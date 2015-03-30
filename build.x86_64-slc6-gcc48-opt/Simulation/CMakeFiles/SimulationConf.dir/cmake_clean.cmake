FILE(REMOVE_RECURSE
  "CMakeFiles/SimulationConf"
  "genConf/Simulation/SimulationConf.py"
  "genConf/Simulation/__init__.py"
  "genConf/Simulation/Simulation.confdb"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/SimulationConf.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
