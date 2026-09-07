#!/usr/bin/env bash
# Prikuplja specifikaciju masine u docs/machine.md -- pokreni PRVO.
set -e
OUT=docs/machine.md
{
  echo "# Machine"
  echo
  echo "Generated: $(date -Iseconds)"
  echo
  echo '## CPU'
  echo '```'
  lscpu | grep -E 'Model name|^CPU\(s\)|Core\(s\) per socket|Socket|Thread\(s\) per core|CPU max MHz|CPU min MHz|Flags' | head -20 || true
  echo '```'
  echo
  echo '## Cache'
  echo '```'
  lscpu | grep -i cache || true
  echo
  getconf -a 2>/dev/null | grep -i CACHE || true
  echo '```'
  echo
  echo '## Toolchain / OS'
  echo '```'
  g++ --version | head -1
  cmake --version | head -1
  uname -srmo
  (. /etc/os-release && echo "$PRETTY_NAME") 2>/dev/null || true
  echo '```'
} > "$OUT"
echo "-> $OUT"
