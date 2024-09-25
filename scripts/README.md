# This directory is for helper scripts, like:
**-**: physicseditor_converter.js
This script will take a convex hull plaintext file from the PhysicEditor software,
and convert it to json format and extract the hull data.. add extra points(to complete the path)
and put it in a typescript array float32 data structure
Call like: 
node physicseditor_converter.js 
it will ask for filename with path.. put something like this form
D:/OtherProjects/AquilaCode/Assets/PhysicsEditor/Ship3/ship3-v1-plaintext.txt
it will output a converted file in the same location that you gave as input.. this would be the output
D:/OtherProjects/AquilaCode/Assets/PhysicsEditor/Ship3/ship3-v1-plaintext_convex_sub.json

```

[5.2][493][jacob@jakesbeastmech][/d/OtherProjects/AquilaCode/scripts]
$node physicseditor_converter.js
Enter the filename and path: D:/OtherProjects/AquilaCode/Assets/PhysicsEditor/Ship3/ship3-v1-plaintext.txt
You entered: D:/OtherProjects/AquilaCode/Assets/PhysicsEditor/Ship3/ship3-v1-plaintext.txt
JSON file has been saved as D:/OtherProjects/AquilaCode/Assets/PhysicsEditor/Ship3/ship3-v1-plaintext_convex_sub.json

```

json output file would look like this
```
{
  "sprites": [
    {
      "name": "ship3IsoDL",
      "convexSubPolygons": [
        [
          4.920000000000073, 378.17599999999993, 102.92000000000007,
          364.17599999999993, 89.92000000000007, 397.17599999999993,
          14.920000000000073, 441.17599999999993, -33.07999999999993,
          408.17599999999993, 4.920000000000073, 378.17599999999993
        ],
      ]
    },
  ]
}
```


## How to structure your Angular apps like a Googler
https://www.youtube.com/watch?v=7SDpTOLeqHE