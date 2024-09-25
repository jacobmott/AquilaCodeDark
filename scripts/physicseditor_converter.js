const fs = require("fs");
const readline = require("readline");

let namesAddedSoFar = {};
let fileData = {
  sprites: [],
};

function askQuestion(query) {
  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  return new Promise((resolve) =>
    rl.question(query, (ans) => {
      rl.close();
      resolve(ans);
    }),
  );
}

// Function to parse the convex sub polygons data
function parseConvexSubPolygons(relevantData, name) {
  const lines = relevantData
    .split("\n")
    .map((line) => line.trim())
    .filter((line) => line !== "");

  let array = lines.map((line) => {
    const numbers = line.match(/-?\d+\.?\d*/g);
    const result = numbers ? numbers.map(Number) : [];
    // result.push(result[0], result[1]);
    return result;
  });

  if (namesAddedSoFar[name]) {
    fileData.sprites.forEach((sprite) => {
      if (sprite.name === name) {
        sprite.convexSubPolygons = sprite.convexSubPolygons.concat(array);
      }
    });
  } else {
    namesAddedSoFar[name] = true;
    fileData.sprites.push({ name: name, convexSubPolygons: array });
  }
}

function removeExtension(filename) {
  return filename.substring(0, filename.lastIndexOf(".")) || filename;
}

function processFile(filename, startTag, processFunction) {
  // Read the file synchronously
  const fileContent = fs.readFileSync(filename, "utf8");

  // Split the content into lines
  const lines = fileContent.split("\n");

  let capturing = false;
  let capturedData = [];

  let name = "";
  for (let line of lines) {
    if (line.indexOf("Name:") !== -1) {
      name = line.substring(line.indexOf("Name:") + 5).trim();
    } else if (line.trim() === startTag) {
      capturing = true;
      capturedData = [];
    } else if (capturing) {
      if (line.trim() === "") {
        // Empty line encountered, process the captured data
        processFunction(capturedData.join("\n"), name);
        capturing = false;
      } else {
        capturedData.push(line);
      }
    }
  }

  // In case the file ends without a blank line
  if (capturing && capturedData.length > 0) {
    processFunction(capturedData.join("\n"), name);
  }
}

async function main() {
  const filenameAndPath = await askQuestion("Enter the filename and path: ");
  console.log(`You entered: ${filenameAndPath}`);

  // Read the input file
  fs.readFile(filenameAndPath, "utf8", (err, data) => {
    if (err) {
      console.error("Error reading file:", err);
      return;
    }
    // Parse the data
    processFile(
      filenameAndPath,
      "Convex sub polygons:",
      parseConvexSubPolygons,
    );

    // Convert the object to JSON
    const jsonOutput = JSON.stringify(fileData, null, 2);
    let fileWithPathNoExt = removeExtension(filenameAndPath);
    fileWithPathNoExt += "_convex_sub.json";
    // Write the JSON to a file
    fs.writeFile(fileWithPathNoExt, jsonOutput, (err) => {
      if (err) {
        console.error("Error writing file:", err);
      } else {
        console.log(`JSON file has been saved as ${fileWithPathNoExt}`);
      }
    });
  });
}

main();
