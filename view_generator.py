import os
import re
import sys

def parse_model_file(file_path):
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Ищем namespace с Events в конце
    #events_namespaces = re.finditer(r'namespace\s+(\w+Events)\s*{([^}]*)}', content)
    #events_namespaces = re.finditer(r'namespace\s+Events\s*{([^}]*)}', content)
    events_namespaces = re.finditer(r'namespace\s+Events\s*{([^{}]*(?:{[^}]*}[^{}]*)*)}', content, re.DOTALL)
    events = []
    
    for match in events_namespaces:
        eventContent = match.group(1)
        
        # Ищем все структуры внутри namespace
        #structs = re.finditer(r'struct\s+(\w+Event)\s*{([^}]*)}', content)
        structs = re.finditer(r'\s*struct\s+(\w+Event)\s*{', eventContent, re.DOTALL)
        for struct in structs:
            events.append(struct.group(1))

    uimodels_namespaces = re.finditer(r'namespace\s+UIModel\s*{([^{}]*(?:{[^}]*}[^{}]*)*)}', content, re.DOTALL)
    main_model = ""
    
    for match in uimodels_namespaces:
        UImodelContent = match.group(1)
        main_model = re.search(r'struct\s+(\w+MainModel)\s*{', UImodelContent)
    
    main_model_name = main_model.group(1) if main_model else None
    
    return events, main_model_name


def parse_view_file(file_path):
    if not os.path.exists(file_path):
        return set(), []
    
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Ищем методы обработки событий
    methods = re.finditer(r'void\s+(\w+)\s*\(\s*const\s+Events::(\w+Event)[^)]*\)', content)
    event_methods = set()
    for match in methods:
        event_methods.add(match.group(2))
    
    # Ищем листенеры в конструкторе
    listeners = re.finditer(r'Listen<Events::(\w+Event)>', content)
    listener_events = [match.group(1) for match in listeners]
    
    return event_methods, listener_events


def update_existing_files(header_path, source_path, model_events, base_name):
    # Читаем существующие файлы
    with open(header_path, 'r') as f:
        header_content = f.read()
    with open(source_path, 'r') as f:
        source_content = f.read()

    # Получаем существующие методы
    existing_methods = set()
    for match in re.finditer(r'void\s+(\w+Event)\s*\(\s*const\s+Events::\w+Event[^)]*\)', header_content):
        existing_methods.add(match.group(1))

    methods_to_add = set(model_events) - existing_methods
    methods_to_remove = existing_methods - set(model_events)

    if methods_to_add or methods_to_remove:
        # Обновляем header файл
        # Удаляем ненужные методы
        for method in methods_to_remove:
            header_content = re.sub(
                r'\s+void\s+' + method + r'\s*\([^)]*\);\n',
                '',
                header_content
            )

        # Добавляем новые методы перед закрывающей скобкой класса
        new_methods = '\n'.join(f'        void {event}(const Events::{event}& event);'
                               for event in methods_to_add)
        if new_methods:
            header_content = re.sub(
                r'(\s+\};)\s*\n\s*}\s*//\s*namespace\s+Windows\s*$',
                f'\n{new_methods}\n\\1\n}} // namespace Windows',
                header_content
            )

        # Обновляем source файл
        # Удаляем ненужные методы
        for method in methods_to_remove:
            # Удаляем реализацию метода
            source_content = re.sub(
                r'\s+void\s+' + base_name.capitalize() + r'View::' + method + r'\s*\([^)]*\)\s*{[^}]*}',
                '',
                source_content
            )
            # Удаляем соответствующий листенер из конструктора
            source_content = re.sub(
                r'\s+Event::Listener\s+' + method + r'Listener;[^;]*;[^;]*;[^;]*;',
                '',
                source_content
            )

        # Добавляем новые методы и листенеры
        # Находим конец конструктора
        constructor_end = re.search(r'(\s+})\s*\n\s*void', source_content)
        if constructor_end:
            # Добавляем новые листенеры
            new_listeners = ''
            for event in methods_to_add:
                new_listeners += f'''\n        Event::Listener {event}Listener;
        {event}Listener.Listen<Events::{event}>(
            [this](const Events::{event}& event) {{
                this->{event}(event);
            }});
        evBus->AddListener(std::move({event}Listener));\n\n'''

            source_content = source_content[:constructor_end.start()] + \
                           new_listeners + \
                           source_content[constructor_end.start():]

        # Добавляем реализации новых методов перед закрывающей скобкой namespace
        new_implementations = '\n'.join(
            f'''    void {base_name.capitalize()}View::{event}(const Events::{event}& event) {{

    }}\n'''
            for event in methods_to_add
        )
        if new_implementations:
            source_content = re.sub(
                r'(\s*}}\s*//\s*namespace\s+Windows\s*)$',
                f'\n{new_implementations}\\1',
                source_content
            )

        # Записываем обновленные файлы
        with open(header_path, 'w') as f:
            f.write(header_content)
        with open(source_path, 'w') as f:
            f.write(source_content)


def generate_view_files(model_path, view_path, model_events, main_model_name, base_name):
    header_path = os.path.join(view_path, f"{base_name}_view.h")
    source_path = os.path.join(view_path, f"{base_name}_view.cpp")
    
    existing_methods, existing_listeners = parse_view_file(header_path)
    
    # Определяем методы для добавления и удаления
    methods_to_add = set(model_events) - existing_methods
    methods_to_remove = existing_methods - set(model_events)
    
    if not os.path.exists(header_path):
        # Создаем новый header файл
        header_content = f'''#pragma once
#include "{model_path}"
#include "event_bus/event_bus.h"

namespace Windows {{

    class {base_name.capitalize()}View {{
    public:
        {base_name.capitalize()}View(Event::EventBus* eventBus, UIModel::{main_model_name} {base_name});

        void ShowWindow(bool& check);

    private:
'''
        # Добавляем методы для каждого события
        for event in model_events:
            header_content += f'        void {event}(const Events::{event}& event);\n'
        
        header_content += f'''
        Event::EventBus* evBus;
        UIModel::{main_model_name} data;
    }};

}} // namespace Windows
'''
        
        with open(header_path, 'w') as f:
            f.write(header_content)
        
        # Создаем новый source файл
        source_content = f'''#include "{base_name}_view.h"

namespace Windows {{

    {base_name.capitalize()}View::{base_name.capitalize()}View(Event::EventBus* eventBus, UIModel::{main_model_name} {base_name})
        : evBus(eventBus), data(std::move({base_name})) {{
'''
        
        # Добавляем листенеры для каждого события
        for event in model_events:
            listener = event[0].lower() + event[1:] 
            source_content += f'''        Event::Listener {listener}Listener;
        {listener}Listener.Listen<Events::{event}>(
            [this](const Events::{event}& event) {{
                this->{event}(event);
            }});
        evBus->AddListener(std::move({listener}Listener));

'''
        
        source_content += '''    }

'''
        
        # Добавляем реализации методов
        for event in model_events:
            source_content += f'''    void {base_name.capitalize()}View::{event}(const Events::{event}& event) {{

    }}

'''
        
        source_content += f'''    void {base_name.capitalize()}View::ShowWindow(bool& check) {{
    
    }}

}} // namespace Windows
'''
        
        with open(source_path, 'w') as f:
            f.write(source_content)
    else:
        # Обновляем существующие файлы
        update_existing_files(header_path, source_path, model_events, base_name)
        #pass  # Здесь нужно реализовать обновление существующих файлов

def main():
    if len(sys.argv) != 3:
        print("Usage: python script.py <models_dir> <views_dir>")
        return
    
    models_dir = sys.argv[1]
    views_dir = sys.argv[2]
    
    for filename in os.listdir(models_dir):
        if filename.endswith('_model.h'):
            model_path = os.path.join(models_dir, filename)
            base_name = filename[:-8]  # remove '_model.h'
            
            events, main_model_name = parse_model_file(model_path)
            if events:
                generate_view_files(model_path, views_dir, events, main_model_name, base_name)

if __name__ == "__main__":
    main()