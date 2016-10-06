# Islandora Repo Reporter

Utility module that generates some basic information about an Islandora repository.

## Requirements

* [Islandora](https://github.com/Islandora/islandora)
* [Islandora Solr Search](https://github.com/Islandora/islandora_solr_search)

## Installation

Same as any module.

## Usage

This module produces a report at `admin/reports/islandora_repo_report` similar to the standard Drupal "Status report" containing Islandora-specific information.

Also, this module offers one drush command, `drush ireporepo`. Running this command will print:

* the total number of objects in your repo
* the number of collection objects
* the created dates of the oldest and most recent objects in the repo
* the number of objects indexed in Solr
* the number of objects that have XACML POLICY datastreams
* a list of "Islandora" modules that are enabled

## Maintainer

* [Mark Jordan](https://github.com/mjordan)

## To do

* Add additional reports (use cases welcome)
* Add ability to write Drush command to output to a file
* Add error trapping so we can report Solr or Resource Index query time outs or errors
* Add ability to pregenerate status data using Drush and cache it for use in the GUI report page

## License and Terms of use

* [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
