FILE(REMOVE_RECURSE
  "CMakeFiles/GenerationComponentsList"
  "Generation.components"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/GenerationComponentsList.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
