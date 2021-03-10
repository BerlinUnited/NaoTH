function validate_url(){
  if [[ `wget -S --spider $1  2>&1 | grep 'HTTP/1.1 200 OK'` ]]; then echo "true"; fi
}

url="https://pages.cms.hu-berlin.de/berlinunited/naoth-2020/content.tar.gz"

echo $(validate_url $url)

if [ -n "$(validate_url $url)" ]; then
        wget --no-check-certificate $url -O prev_content.tar.gz;
        if [ -f prev_content.tar.gz ]; then 
        	tar -xzf prev_content.tar.gz public/; 
        fi
else
        echo "remote url does not exist";
fi

new_branch_name="$(sed 's/\//_/g' <<< "${CI_COMMIT_REF_NAME}")"
# rename folders like feature/my_cool_feature to feature_my_cool_feature
if [ "$CI_COMMIT_REF_NAME" != "$new_branch_name" ]; then mv $CI_COMMIT_REF_NAME $new_branch_name; else new_branch_name=$CI_COMMIT_REF_NAME; fi
mkdir -p public/$new_branch_name
cp -r $CI_COMMIT_REF_NAME/* public/$new_branch_name/
tar -zcf content.tar.gz public/

# copy the content.tar.gz inside public folder so its part of the pages deployment
cp content.tar.gz public/