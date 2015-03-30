FILE(REMOVE_RECURSE
  "CMakeFiles/ExamplesComponentsList"
  "Examples.components"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/ExamplesComponentsList.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
