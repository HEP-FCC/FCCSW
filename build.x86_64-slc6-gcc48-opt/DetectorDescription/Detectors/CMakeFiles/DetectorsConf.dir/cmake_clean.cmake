FILE(REMOVE_RECURSE
  "CMakeFiles/DetectorsConf"
  "genConf/Detectors/DetectorsConf.py"
  "genConf/Detectors/__init__.py"
  "genConf/Detectors/Detectors.confdb"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/DetectorsConf.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
