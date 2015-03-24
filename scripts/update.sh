cd ..
git pull
git submodule foreach git pull origin master
git add -u
git commit -m "Update $(date)"
git push

