FILE(REMOVE_RECURSE
  "CMakeFiles/doxygen_target"
  "xml"
  "html"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/doxygen_target.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
