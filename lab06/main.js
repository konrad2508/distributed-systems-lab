const zookeeper = require('node-zookeeper-client');
const inquirer = require('inquirer');
const pressAnyKey = require('press-any-key');
const { execFile } = require('child_process');
const ps = require('ps-node');

let client;


const onCreateZ = () => {
    execFile('notepad', (error, stdout, stderr) => {
        if (error) {
            throw error;
        }
        console.log(stdout);
    });
};


const onDeleteZ = () => {
    execFile('taskkill /im notepad.exe', (error, stdout, stderr) => {
        // if (error) {
        //     throw error;
        // }
        // console.log(stdout);
    });
};


const createZ = async () => {
    client.create(
        '/z',
        null,
        zookeeper.CreateMode.PERSISTENT,
        (error, path) => {
            if (error) {
                console.log(error.stack);
                return;
            }

            client.exists('/z', onDeleteZ, () => {});
            console.log('Node: %s is created.', path);
        }
    );

    await pressAnyKey('Press any key to continue...').then(console.clear);
};


const deleteZ = async () => {
    client.getChildren('/z', null, (error, children, stats) => {
        children.forEach((child) => {
            client.remove(`/z/${child}`, -1, () => {
            })
        });

        client.remove('/z', -1, (error) => {
            if (error) {
                console.log(error.stack);
                return;
            }

            client.exists('/z', onCreateZ, () => {});
            console.log('Node is deleted.');
        });
    });

    await pressAnyKey('Press any key to continue...').then(console.clear);
};


const createChild = async (name) => {
    client.create(
        `/z/${name}`,
        null,
        zookeeper.CreateMode.PERSISTENT,
        (error, path) => {
            if (error) {
                console.log(error.stack);
                return;
            }

            console.log('Node: %s is created.', path);
        }
    );

    await pressAnyKey('Press any key to continue...').then(console.clear);
};


const viewTree = async () => {
    client.getChildren('/z', null, (error, children, stats) => {
        if (error) {
            console.log(error.stack);
            return;
        }

        console.log('Children are: %j.', children);
        console.log();
        console.log(stats);
    });

    await pressAnyKey('Press any key to continue...').then(console.clear);
};


(async function () {
    console.clear();

    client = zookeeper.createClient('localhost:2181');
    client.connect();
    client.exists('/z', onCreateZ, (err) => {
        if (err) console.log(err);
    });

    let loop = true;
    while (loop) {
        let choice = null;
        await inquirer.prompt([
            {
                type: 'list',
                message: 'Select command',
                name: 'choice',
                choices: [
                    'Create',
                    'Delete',
                    'Add child',
                    'View tree',
                    'Quit'
                ]
            }
        ]).then(answer => {
            choice = answer.choice;
        });

        switch (choice) {
            case 'Create':
                await createZ();

                break;
            case 'Delete':
                await deleteZ();

                break;
            case 'Add child':
                let name;
                await inquirer.prompt([
                    {
                        type: 'text',
                        message: 'Enter new node name',
                        name: 'name'
                    },
                ]).then(answer => {
                    name = answer.name;
                });
                await createChild(name);

                break;
            case 'View tree':
                await viewTree();

                break;
            case 'Quit':
                loop = false;

                break;
            default:
        }
    }
    client.close();
}());