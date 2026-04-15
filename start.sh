#!/usr/bin/env bash
set -e

export PATH="/opt/cmake/bin:$PATH"
cd /home/birchstonereporting/Construction-Summary-Engine
source /home/birchstonereporting/Construction-Summary-Engine/venv/bin/activate
exec python3 /home/birchstonereporting/Construction-Summary-Engine/scripts/oversee_process.py