FILE(REMOVE_RECURSE
  "CMakeFiles/GenerationConf"
  "genConf/Generation/GenerationConf.py"
  "genConf/Generation/__init__.py"
  "genConf/Generation/Generation.confdb"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/GenerationConf.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
