FILE(REMOVE_RECURSE
  "CMakeFiles/DetectorsComponentsList"
  "Detectors.components"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/DetectorsComponentsList.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
