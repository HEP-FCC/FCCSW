FILE(REMOVE_RECURSE
  "CMakeFiles/FWCorePluginsConf"
  "genConf/FWCore/FWCorePluginsConf.py"
  "genConf/FWCore/__init__.py"
  "genConf/FWCore/FWCorePlugins.confdb"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/FWCorePluginsConf.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
