#!/usr/bin/env bash
# Formats every C++ file in the repository in place with clang-format.
#
# Usage: scripts/format.sh [--check]
#
# With --check nothing is changed, and the script fails if any file is not formatted.
# CI runs it that way.
set -euo pipefail
cd "$(dirname "$0")/.."

mode="${1:-}"
if [[ $# -gt 1 || ( -n "$mode" && "$mode" != "--check" ) ]]; then
    echo "usage: scripts/format.sh [--check]" >&2
    exit 2
fi

# Tracked files plus new ones that are not ignored.
mapfile -t files < <(git ls-files --cached --others --exclude-standard '*.h' '*.cpp')

if [[ "$mode" == "--check" ]]; then
    clang-format --dry-run --Werror "${files[@]}"
else
    clang-format -i "${files[@]}"
fi
