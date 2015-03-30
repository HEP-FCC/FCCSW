FILE(REMOVE_RECURSE
  "CMakeFiles/MergedComponentsList_force"
  "lib/FCCSW.components_force"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/MergedComponentsList_force.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
