FILE(REMOVE_RECURSE
  "CMakeFiles/TestToolsConf"
  "genConf/TestTools/TestToolsConf.py"
  "genConf/TestTools/__init__.py"
  "genConf/TestTools/TestTools.confdb"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/TestToolsConf.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
