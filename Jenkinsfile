pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh "mkdir build || true; cd build && cmake .. && make;"
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
                sh "cd build && make test"
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying.... (skip)'
            }
        }
    }
}
