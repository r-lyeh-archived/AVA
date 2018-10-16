cd /d "%~dp0"
cd ..

git add -Af && git commit --allow-empty-message -m ""
git push origin HEAD:master %*
REM git checkout master --
