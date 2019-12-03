#!/usr/bin/sh

echo "Syncing on cluster"

# shellcheck disable=SC2039
# shellcheck disable=SC2034
for I in {1..15}
do
  echo "~> working on server $I"
  rsync -azP ~/.ssh/authorized_keys "$USER"@dim-openmpi$I.uqac.ca:~/.ssh/authorized_keys &
  rsync -azP "$PWD/" "$USER"@dim-openmpi$I.uqac.ca:$PWD &
done