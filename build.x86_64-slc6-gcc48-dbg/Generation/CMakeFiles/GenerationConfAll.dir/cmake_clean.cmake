FILE(REMOVE_RECURSE
  "CMakeFiles/GenerationConfAll"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/GenerationConfAll.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
