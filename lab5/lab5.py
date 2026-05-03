import streamlit as st
import pandas as pd
import os
import io
from datetime import datetime
from urllib.request import urlopen

# Очещена і систематизована база данних з 2 лабораторної:
NAMES = {1: "Vinnytsia", 2: "Volyn", 3: "Dnipropetrovsk", 4: "Donetsk", 5: "Zhytomyr",
         6: "Zakarpattia", 7: "Zaporizhzhia", 8: "Ivano-Frankivsk", 9: "Kyiv",
         10: "Kirovohrad", 11: "Luhansk", 12: "Lviv", 13: "Mykolaiv", 14: "Odesa",
         15: "Poltava", 16: "Rivne", 17: "Sumy", 18: "Ternopil", 19: "Kharkiv",
         20: "Kherson", 21: "Khmelnytskyi", 22: "Cherkasy", 23: "Chernivtsi",
         24: "Chernihiv", 25: "Crimea"}

ID_MAP = {1: 22, 2: 24, 3: 23, 4: 25, 5: 3, 6: 4, 7: 8, 8: 19, 9: 20, 10: 21,
          11: 9, 12: 13, 13: 14, 14: 15, 15: 16, 16: 17, 17: 18, 18: 6, 19: 1,
          20: 2, 21: 7, 22: 5, 23: 10, 24: 11, 25: 12}

@st.cache_data
def download_vhi_data():
    if not os.path.exists('data'):
        os.makedirs('data')
    for province_id in range(1, 26):
        url = f"https://www.star.nesdis.noaa.gov/smcd/emb/vci/VH/get_TS_admin.php?country=UKR&provinceID={province_id}&year1=1981&year2=2024&type=Mean"
        existing_files = [f for f in os.listdir('data') if f.startswith(f'vhi_id_{province_id}_')]
        if not existing_files:
            try:
                with urlopen(url) as response:
                    content = response.read().decode('utf-8')
                now = datetime.now().strftime("%Y%m%d_%H%M%S")
                filename = f"data/vhi_id_{province_id}_{now}.csv"
                with open(filename, 'w', encoding='utf-8') as f:
                    f.write(content)
            except Exception as e:
                pass

def create_clean_dataframe(folder_path):
    all_data = []
    if not os.path.exists(folder_path): return pd.DataFrame()
    files = [f for f in os.listdir(folder_path) if f.startswith('vhi_id_') and f.endswith('.csv')]
    for file in files:
        try:
            noaa_id = int(file.split('_')[2])
            df = pd.read_csv(os.path.join(folder_path, file), header=1, 
                             names=['Year', 'Week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI', 'extra'])
            df = df.drop(columns=['extra']).dropna()
            df['Year'] = df['Year'].astype(str).str.replace('<tt><pre>', '', regex=False).str.replace('</pre></tt>', '', regex=False)
            df = df[df['VHI'] != -1]
            df['Area_ID'] = ID_MAP[noaa_id]
            all_data.append(df)
        except: continue
    return pd.concat(all_data, ignore_index=True) if all_data else pd.DataFrame()



#     логіка пункту 5: Button для скидання
if 'reset_key' not in st.session_state:
    st.session_state.reset_key = 0

def reset_callback():
    st.session_state.reset_key += 1
    for key in list(st.session_state.keys()):
        if key != 'reset_key':
            del st.session_state[key]

download_vhi_data()
df = create_clean_dataframe('data')
df['Region'] = df['Area_ID'].map(NAMES)
df['Year'] = pd.to_numeric(df['Year']).astype(int)
df['VHI'] = df['VHI'].astype(float)


# 5. Інтерфейс Streamlit
st.set_page_config(layout="wide")
col_filters, col_content = st.columns([1, 3])

with col_filters:
    st.header("Фільтри")
    suffix = st.session_state.reset_key
        # Пункт 1: dropdown список, який дозволяє вибрати часовий ряд VCI, TCI, VHI;
    index_name = st.selectbox("Оберіть часовий ряд:", ["VHI", "TCI", "VCI"], key=f"idx_{suffix}")
        # Пункт 2: dropdown список, який дозволяє вибрати область, для якої буде  виконуватись аналіз; 
    selected_reg = st.selectbox("Оберіть область:", sorted(df['Region'].unique()), key=f"reg_{suffix}")
        # Пункт 3: Slider, який дозволяє вибирати інтервал тижнів, за які відбираються дані;  
    weeks = st.slider("Інтервал тижнів:", 1, 52, (1, 52), key=f"weeks_{suffix}")
        # Пункт 4: Slider, який дозволяє вибрати інтервал років, за які відбираються дані; 
    years = st.slider("Інтервал років:", int(df['Year'].min()), int(df['Year'].max()), (2000, 2024), key=f"years_{suffix}")

        # Пункт 8: Checkboxes для сортування даних за зростанням та спаданням значень  VCI, TCI або VHІ;
    st.write("Сортування за значенням:")
    asc = st.checkbox("За зростанням", key=f"asc_{suffix}")
    desc = st.checkbox("За спаданням", key=f"desc_{suffix}")
        # Пункт 5: Button для скидання
    st.button("Скинути всі фільтри", on_click=reset_callback)

#  Фільтрація та сортування
mask = (df['Region'] == selected_reg) & (df['Year'].between(*years)) & (df['Week'].between(*weeks))
filtered_df = df[mask].copy()

    # Пункт 8: Продумайте реакцію  програми, якщо увімкнені обидва чекбокси;
if asc and desc:
    st.warning("Обрано обидва типи сортування. Використовується за спаданням.")
    filtered_df = filtered_df.sort_values(by=index_name, ascending=False)
elif asc:
    filtered_df = filtered_df.sort_values(by=index_name, ascending=True)
elif desc:
    filtered_df = filtered_df.sort_values(by=index_name, ascending=False)

    # Пункт 6: Створіть три вкладки для відображення таблиці з відфільтрованими даними, відповідного 
    #до неї графіка та графіка порівняння даних по областях;
with col_content:
    t_table, t_chart, t_compare = st.tabs(["Таблиця", "Графік області", "Порівняння"])

    with t_table:
        st.subheader(f"Дані для {selected_reg}")
        st.dataframe(filtered_df, use_container_width=True)

    with t_chart:
        st.subheader(f"Динаміка {index_name}")
        line_data = filtered_df.sort_values(['Year', 'Week'])
        st.line_chart(line_data.set_index('Year')[index_name])

    with t_compare:
        st.subheader(f"Середній {index_name} по регіонах")
        comp_data = df[df['Year'].between(*years) & df['Week'].between(*weeks)]
        comp_result = comp_data.groupby('Region')[index_name].mean().sort_values()
        st.bar_chart(comp_result)
