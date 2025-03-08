const fs = require("fs")

// This code is cursed af but it works :)

/** @type { Record<string, { name: string, command: string, description: string, }[]> } */
let info = []
let defaultClassNames = []

parseBaseClassNames()
function parseBaseClassNames(path = `${process.cwd()}/../LX/src/commands/types`) {
    for (const entry of fs.readdirSync(path)) {
        if (fs.statSync(`${path}/${entry}`).isDirectory()) {
            parseBaseClassNames(`${path}/${entry}`)
        }
        else defaultClassNames.push(entry.slice(0, entry.lastIndexOf(".")))
    }
}

function parseFile(path, folderName = "") {
    let data = fs.readFileSync(path).toString()

    let classNames = [...defaultClassNames]
    const words = data.split(/\r\n|\n| +/).map(word => word.replace(/\t/g, ""))
    for (let i = 0; i < words.length; i++) {
        if (words[i] === "class") {
            classNames.push(words[++i])
        }

        if (classNames.includes(words[i]) && words[i + 1].startsWith("g_")) {
            let line = ""
            while (!line.trim().endsWith(";")) {
                line += words[++i] + " "
            }

            let stringIndex = -1
            let copying = false
            let strings = []
            for (const char of line) {
                if (!copying && strings.length >= 3)
                    break;

                if (!copying) {
                    if (char === '"' || char === "{") {
                        copying = true
                        stringIndex++
                    }
                }
                else {
                    if (char === '"' || char === "}") {
                        copying = false
                    }
                    else {
                        strings[stringIndex] ??= ""
                        strings[stringIndex] += char
                    }
                }
            }

            info[folderName] ??= []
            info[folderName].push({
                name: strings[0] ?? "Name not found",
                command: strings[1] ?? "Command not found",
                description: strings[2]?.replace(/{|}/g, '"') || "No Description"
            })
        }
    }
}

let commandsPath = `${process.cwd()}/../LX/src/commands/commands`
function parseFolder(path = commandsPath, folderName = "") {
    for (const entry of fs.readdirSync(path)) {
        if (fs.statSync(`${path}/${entry}`).isDirectory()) {
            parseFolder(`${path}/${entry}`, folderName || entry)
        }
        else parseFile(`${path}/${entry}`, folderName)
    }
}

parseFolder()

// Parsed Data can be used here. info is a array containing all the command data. type: Record<string, { name: string, command: string, description: string }[]>
let output = ""
for (const [folderName, commands] of Object.entries(info)) {
    output += `# ${folderName[0].toUpperCase() + folderName.slice(1)}\n`
    for (const command of commands) {
        output += `## ${command.name}\n\tCommand: ${command.command}\n\tDescription: ${command.description}\n\n`
    }
}

fs.writeFileSync(`${process.cwd()}/commands_list.md`, output.trim())