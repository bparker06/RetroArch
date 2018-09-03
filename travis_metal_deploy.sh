#!/bin/bash

mkdir -p ~/.ssh

chmod 700 ~/.ssh

echo "Copying SSH key..."

travis pubkey -r ${TRAVIS_REPO_SLUG} >>~/.ssh/authorized_keys

echo "SSH Authorized Keys:"

cat ~/.ssh/authorized_keys

cd ${TRAVIS_BUILD_DIR}/pkg/apple/build/Release

FILENAME=$(date +%F)_RetroArch.dmg

echo "Creating DMG image..."

hdiutil create -volname RetroArch -srcfolder ./ -ov -format UDZO ${FILENAME}

echo "Uploading to server..."

rsync -avhP -e 'ssh -p 12346 -o StrictHostKeyChecking=no' ${FILENAME} travis@bot.libretro.com:~/
