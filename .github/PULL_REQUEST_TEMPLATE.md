_Check if you branch history is PR compatible_
- Your branch need to be up to date with origin/master AND to have linear history (i.e. no merge commit).
- Update your git repository `git fetch origin` if origin is this remote
- Check with the script provided in `scripts/is-history-pr-compatible.sh`
- You must use clang-format style
_These checks are enforced by github workflow actions_
_Please refer to the corresponding log in case of failure_

_UPDATE the form below to describe your PR._

* **Please check if the PR fulfills these requirements**
- [ ] The commit message follows our guidelines
- [ ] Tests for the changes have been added (for bug fixes / features)
- [ ] Docs have been added / updated (for bug fixes / features)

Be aware that the PR request cannot be accepted if it doesn't pass the Continuous Integration tests.


* **What kind of change does this PR introduce?** (Bug fix, feature, docs update, ...)



* **What is the current behavior?** (You can also link to an open issue here)



* **What is the new behavior (if this is a feature change)?**



* **Does this PR introduce a breaking change?** (What changes might users need to make in their application due to this PR?)



* **Other information**:
