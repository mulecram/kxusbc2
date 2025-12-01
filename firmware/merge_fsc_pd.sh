#!/bin/sh

# This script expects the release v4.1.2 ZIP of the FUSB302 reference code from onsemi,
# which can be downloaded from (login required):
# https://www.onsemi.com/design/evaluation-board/FUSB302BGEVB
# Place the ZIP file (FUSB302 REFERENCE CODE.ZIP) in the same directory as this script before running it.
# The script will extract the necessary files into src/fsc_pd, apply a patch, and optionally clean up.

set -e

ZIPFILE="FUSB302 REFERENCE CODE.ZIP"
UNPACKED="Release_v4.1.2"
TARGETDIR="src/fsc_pd"
PATCHFILE="fsc_pd.patch"

# Check for ZIP file
if [ ! -f "$ZIPFILE" ]; then
  echo "Error: $ZIPFILE not found. Please download it from onsemi."
  exit 1
fi

# Ensure target directory exists
mkdir -p "$TARGETDIR"

# Unpack ZIP
unzip -q "$ZIPFILE"

# Check unpacked folder
if [ ! -d "$UNPACKED" ]; then
  echo "Error: Unpacked folder '$UNPACKED' not found."
  exit 1
fi

# List of files to copy (relative to $UNPACKED)
FILES="
core/fusb30X.c
core/core.c
core/core.h
core/modules/dpm.c
core/modules/dpm.h
core/fusb30X.h
core/Log.c
core/Log.h
core/modules/observer.c
core/modules/observer.h
core/PD_Types.h
core/PDPolicy.c
core/PDPolicy.h
core/PDProtocol.c
core/PDProtocol.h
core/Port.c
core/Port.h
core/timer.c
core/timer.h
core/TypeC_Types.h
core/TypeC.c
core/TypeC.h
core/vdm/vdm_types.h
core/vendor_info.c
core/version.h
core/vif_macros.h
Platform_None/FSCTypes.h
"

for relpath in $FILES; do
  src="$UNPACKED/$relpath"
  dest="$TARGETDIR/$(basename "$relpath")"
  if [ ! -f "$src" ]; then
    echo "Error: $src not found."
    exit 1
  fi
  cp "$src" "$dest"
done

# Apply patch
if [ ! -f "$PATCHFILE" ]; then
  echo "Error: Patch file $PATCHFILE not found."
  exit 1
fi

patch -d "$TARGETDIR" -p1 < "$PATCHFILE"

# Prompt before cleaning up unpacked folder
read -p "Remove the unpacked folder '$UNPACKED'? [y/N] " confirm
if [ "$confirm" = "y" ] || [ "$confirm" = "Y" ]; then
  rm -rf "$UNPACKED"
  echo "Removed '$UNPACKED'."
else
  echo "Unpacked folder '$UNPACKED' left in place."
fi

echo "Done."
