const zookeeper = require('node-zookeeper-client');
const inquirer = require('inquirer');
const pressAnyKey = require('press-any-key');


(async function () {
    console.clear();

    let loop = true;
    while (loop) {
        let choice = null;
        await inquirer.prompt([
            {
                type: 'list',
                message: 'Select command',
                name: 'choice',
                choices: [
                    'Hello',
                    'Quit'
                ]
            }
        ]).then(answer => {
            choice = answer.choice;
        });

        switch (choice){
            case 'Hello':
                console.log('\nHello\n');
                await pressAnyKey('Press any key to continue...').then(console.clear);

                break;
            case 'Quit':
                loop = false;

                break;
            default:
        }
    }
}());