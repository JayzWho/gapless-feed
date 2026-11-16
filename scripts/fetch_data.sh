#!/usr/bin/env bash
# Downloads the Nasdaq TotalView-ITCH 5.0 sample this project runs on, checks it, and
# decompresses it next to the archive.
#
# Usage: scripts/fetch_data.sh [target-dir]        default target: Data/ in the repository
#
# The download is 4.8 GB and the decompressed file about 11 GB. Safe to re-run: an
# interrupted download resumes, and steps that are already done are skipped.
#
# ITCH_URL and ITCH_BYTES select another file from the same directory listing:
#   https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/
set -euo pipefail

url="${ITCH_URL:-https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/01302019.NASDAQ_ITCH50.gz}"
expected_bytes="${ITCH_BYTES:-4764426091}"

repo_root="$(cd "$(dirname "$0")/.." && pwd)"
target_dir="${1:-$repo_root/Data}"
archive="${url##*/}"
decoded="${archive%.gz}"

mkdir -p "$target_dir"
cd "$target_dir"

size_of() {
    stat -c %s "$1" 2>/dev/null || echo 0
}

if [[ "$(size_of "$archive")" != "$expected_bytes" ]]; then
    echo "Downloading $archive"
    wget --continue --no-verbose --show-progress "$url"
fi

# The listing publishes no usable checksum, so the checks are the exact size and the
# CRC that gzip stores in the archive.
actual_bytes="$(size_of "$archive")"
if [[ "$actual_bytes" != "$expected_bytes" ]]; then
    echo "error: $archive is $actual_bytes bytes, expected $expected_bytes" >&2
    exit 1
fi
echo "Checking $archive"
gzip --test "$archive"

if [[ ! -f "$decoded" ]]; then
    echo "Decompressing to $decoded"
    # Written under a temporary name, so an interrupted run never leaves a partial file
    # that looks complete.
    gzip --decompress --stdout "$archive" > "$decoded.part"
    mv "$decoded.part" "$decoded"
fi

echo "Ready:"
ls -l "$PWD/$archive" "$PWD/$decoded"
