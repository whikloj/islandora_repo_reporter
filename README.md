# Islandora Repo Reporter

Utility module that generates some basic information about an Islandora repository.

## Requirements

* [Islandora](https://github.com/Islandora/islandora)

## Installation

Same as any module.

## Usage

This module produces a report at `admin/reports/islandora_repo_report` similar to the standard Drupal "Status report" containing Islandora-specific information.

Also, this module offers one drush command, `drush ireporepo`. Running this command will print:

* the total number of objects in your repo
* the number of collection objects
* the created dates of the oldest and most recent objects in the repo
* a list of modules that are enabled and whose machine names begin with 'islandora'

## Maintainer

* [Mark Jordan](https://github.com/mjordan)

## To do

* Add reports on:
  * Installed content models
  * XACML policies in effect

## License and Terms of use

* [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
