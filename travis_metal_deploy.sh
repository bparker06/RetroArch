#!/bin/bash

mkdir -p ~/.ssh

chmod 700 ~/.ssh

openssl aes-256-cbc -K $encrypted_e9bb4da59666_key -iv $encrypted_e9bb4da59666_iv -in travis-deploy-key.enc -out ~/.ssh/id_rsa -d

chmod 600 ~/.ssh/id_rsa

cd ${TRAVIS_BUILD_DIR}/pkg/apple/build/Release

FILENAME=$(date +%F)_RetroArch.dmg

echo "Creating DMG image..."

hdiutil create -volname RetroArch -srcfolder ./ -ov -format UDZO ${FILENAME}

echo "Uploading to server..."

rsync -avhP -e 'ssh -p 12346 -o StrictHostKeyChecking=no' ${FILENAME} travis@bot.libretro.com:~/
