#!/bin/bash

#set -u

DEFAULT_POLL_TIMEOUT=10
POLL_TIMEOUT=${POLL_TIMEOUT:-$DEFAULT_POLL_TIMEOUT}

echo ${GITHUB_REF}
# leave ancient history undisturbed
git fetch --shallow-since=01/01/2020

#github_ref
#git checkout "${GITHUB_REF}"

#branch=${GITHUB_REPOSITORY}/$(git symbolic-ref --short HEAD)
shortbranch=$(git symbolic-ref --short HEAD)

#git config --global credential.username $GITLAB_USERNAME
#git config --global core.askPass .github/scripts/cred-helper.sh
#git config --global credential.helper cache
#git remote add mirror $GITLAB_URL
#echo pushing to $shortbranch branch at $(git remote get-url --push mirror)
#git push mirror $shortbranch


# need a token with api acces
curl --silent   --header "PRIVATE-TOKEN: ${GITLAB_PASSWORD2}" -X https://${GITLAB_HOSTNAME}/api/v4/projects/${GITLAB_PROJECT_ID}/mirror/pull

sleep $POLL_TIMEOUT


pipeline_id=$(curl --silent "https://${GITLAB_HOSTNAME}/api/v4/projects/${GITLAB_PROJECT_ID}/repository/commits/${shortbranch}" | jq '.last_pipeline.id')

echo "Triggered CI for branch ${shortbranch}"
echo "Working with pipeline id #${pipeline_id}"
echo "Poll timeout set to ${POLL_TIMEOUT}"

ci_status="pending"

until [[ "$ci_status" != "pending" && "$ci_status" != "running" ]]
do
   sleep $POLL_TIMEOUT
   ci_output=$(curl --silent "https://${GITLAB_HOSTNAME}/api/v4/projects/${GITLAB_PROJECT_ID}/pipelines/${pipeline_id}")
   ci_status=$(jq -n "$ci_output" | jq -r .status)
   ci_web_url=$(jq -n "$ci_output" | jq -r .web_url)
   
   echo "Current pipeline status: ${ci_status}"
   if [ "$ci_status" = "running" ]
   then
     echo "Checking pipeline status..."
     curl -d '{"state":"pending", "target_url": "'${ci_web_url}'", "context": "gitlab-ci"}' -H "Authorization: token ${GITHUB_TOKEN}"  -H "Accept: application/vnd.github.antiope-preview+json" -X POST --silent "https://api.github.com/repos/${GITHUB_REPOSITORY}/statuses/${GITHUB_SHA}"  > /dev/null 
   fi
done

echo "Pipeline finished with status ${ci_status}"
  
if [ "$ci_status" = "success" ]
then 
  curl -d '{"state":"success", "target_url": "'${ci_web_url}'", "context": "gitlab-ci"}' -H "Authorization: token ${GITHUB_TOKEN}"  -H "Accept: application/vnd.github.antiope-preview+json" -X POST --silent "https://api.github.com/repos/${GITHUB_REPOSITORY}/statuses/${GITHUB_SHA}" 
  exit 0
elif [ "$ci_status" = "failed" ]
then 
  curl -d '{"state":"failure", "target_url": "'${ci_web_url}'", "context": "gitlab-ci"}' -H "Authorization: token ${GITHUB_TOKEN}"  -H "Accept: application/vnd.github.antiope-preview+json" -X POST --silent "https://api.github.com/repos/${GITHUB_REPOSITORY}/statuses/${GITHUB_SHA}" 
  exit 1
fi
