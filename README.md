# cms

## Usage

## cms --publish <path> [<tags>] [<origin>] [--delete] [--preview]

## cms --extract <guid> <path>

## cms --extractAll <tag> <path>

## cms --refs <path>
Update all asset references under path `<path>`.  This adds or removes linkages from usages to assets, as well as flags untracked assets in these paths.

## cms --manual
React to manual changes to database SST files.  Usages can _only_ be added, edited, or removed manually.  Assets can _only_ be edited or removed manually.
