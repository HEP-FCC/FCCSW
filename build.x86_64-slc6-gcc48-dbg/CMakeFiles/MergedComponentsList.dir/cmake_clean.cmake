FILE(REMOVE_RECURSE
  "CMakeFiles/MergedComponentsList"
  "lib/FCCSW.components"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/MergedComponentsList.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
