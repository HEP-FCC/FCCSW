FILE(REMOVE_RECURSE
  "CMakeFiles/ReconstructionConf"
  "genConf/Reconstruction/ReconstructionConf.py"
  "genConf/Reconstruction/__init__.py"
  "genConf/Reconstruction/Reconstruction.confdb"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/ReconstructionConf.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
