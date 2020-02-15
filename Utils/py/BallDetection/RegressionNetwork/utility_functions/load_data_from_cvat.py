import requests


def get_task_by_id(auth_cookies, id):
    resp = requests.get('http://141.20.26.37:8080/api/v1/tasks?id=55', cookies=auth_cookies)
    if resp.status_code != 200:
        # This means something went wrong.
        print("HTTP %i - %s, Message %s" % (resp.status_code, resp.reason, resp.text))

    print(resp.json())


def get_task_ids(auth_cookies):
    task_id_list = list()

    resp = requests.get('http://141.20.26.37:8080/api/v1/tasks', cookies=auth_cookies)
    if resp.status_code != 200:
        # This means something went wrong.
        print("HTTP %i - %s, Message %s" % (resp.status_code, resp.reason, resp.text))

    json_response = resp.json()
    next_page = json_response['next']
    print(next_page)

    for entry in json_response['results']:
        task_id_list.append(entry['id'])

    while next_page is not None:
        resp = requests.get(next_page, cookies=auth_cookies)
        if resp.status_code != 200:
            # This means something went wrong.
            print("HTTP %i - %s, Message %s" % (resp.status_code, resp.reason, resp.text))

        json_response = resp.json()
        next_page = json_response['next']
        print(next_page)

        for entry in json_response['results']:
            task_id_list.append(entry['id'])

    print(task_id_list)


if __name__ == "__main__":
    # TODO get username and password via argparse
    data = {'username': 'benji', 'password': '7QCQAh49FDBE'}

    resp = requests.post('http://141.20.26.37:8080/api/v1/auth/login', data=data)
    if resp.status_code != 200:
        # This means something went wrong.
        print("HTTP %i - %s, Message %s" % (resp.status_code, resp.reason, resp.text))

    auth_cookies = resp.cookies

    get_task_ids(auth_cookies)
