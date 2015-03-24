# Scripts

The `./update.sh` script is to update this repository. It is meant for collaborators.

As user take a look at the [AIM](http://dobots.github.io/aim/) website. Also, the different AI modules themselves come often with scripts.

# Update a git submodule

Do not forget if you update a module in the main repository to feed back the changes to the individual repository.

For example:

* you have found a bug in `rur-builder`
* cd `rur-builder` and you start working on repairing the code
* you're done and want to submit your code
* create a new branch: `git checkout -b nameforyourpatch`
* update remote code: `git push -u origin nameforyourpatch`

To update the module itself subsequently, you can use the (code in the) `update.sh` script.

Check https://github.com/dobots/rur-builder after you're done if everything went correctly.

