FILE(REMOVE_RECURSE
  "CMakeFiles/ExamplesConf"
  "genConf/Examples/ExamplesConf.py"
  "genConf/Examples/__init__.py"
  "genConf/Examples/Examples.confdb"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/ExamplesConf.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
