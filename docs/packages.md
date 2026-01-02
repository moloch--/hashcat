
This page contains information about packages for hashcat, with guidance [for users](#for-users) and [for package maintainers](#for-package-maintainers).

## For users ##

Many OS packaging systems have a package simply called 'hashcat'.

### Downstream package status ###

Here is a list of downstream packages that include hashcat, as tracked by [Repology](https://repology.org).

[![Packaging status](https://repology.org/badge/vertical-allrepos/hashcat.svg)](https://repology.org/project/hashcat/versions)

## For package maintainers ##

### Packaging guidance ###

If needed, you can disable hardcoded CPU optimization flags with a MAINTAINER_MODE flag.

Compile hashcat with `make MAINTAINER_MODE=1` instead of just `make`.
