



def options_get(env, name):
    ''' Функция чтения параметра из файла конфигурации PlatformIO IDE.
    name - Строка с именем параметра.
    В случае успеха возвращает строку со значением параметра.
    В случае ошибки прерывает выполнение скрипта и выводит сообщение.'''
    try:
        return env.GetProjectOption(name)
    except:
        print(f"Error: '{name}' non specifed!")
        exit(1)


def get_define(file: list, define_name: str)->str:
    ''' The function finds the '#define' directive with the title 'define_name'\
and if it exists, returns the value, otherwise returns 'None'.'''
    for i in range(len(file)):
        if file[i].find(f'#define {define_name}') != -1:
            line_list = file[i].split(' ')
            if len(line_list) >= 3:
                return line_list[2].replace('\n', ' ')

    return None

def set_define(file: list, define_name: str, value: str):
    '''The function finds the '#define' directive with title 'define_name' \
and if it exist change value at 'value'.'''
    for i in range(len(file)):
        if file[i].find(f'#define {define_name}') != -1:
            file[i] = f'#define {define_name} {value}\n'
            break

def add_define(file: list, define_name: str, value: str):
    pass


def get_firmware_version(env) -> list | None:
    """
    Возвращает лист [major, minor] или None.
    """
    try:
        return [env.GetProjectOption('board_version.major'), env.GetProjectOption('board_version.minor')]
    except:
        return None