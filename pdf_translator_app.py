import os
import io
from typing import List

import streamlit as st
from openai import OpenAI
from pypdf import PdfReader
from fpdf import FPDF

# Seitenkonfiguration und einfacher Stil
st.set_page_config(page_title="PDF Übersetzer", page_icon="📄", layout="wide")

st.title("📄 PDF Übersetzer")
st.markdown("Lade eine PDF-Datei hoch und übersetze sie in eine beliebige Zielsprache.")

# Eingabe für API Key
api_key = st.text_input("302.api Schlüssel", type="password")

def extract_text(file: io.BytesIO) -> str:
    reader = PdfReader(file)
    text = []
    for page in reader.pages:
        content = page.extract_text() or ""
        text.append(content)
    return "\n".join(text)


def translate_text(client: OpenAI, text: str, target_language: str) -> str:
    prompt = (
        f"Bitte übersetze den folgenden Text in {target_language} und erhalte das Format bei:\n\n{text}"
    )
    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[{"role": "user", "content": prompt}],
        temperature=0.2,
    )
    return response.choices[0].message.content.strip()


def chunk_text(text: str, size: int = 2000) -> List[str]:
    return [text[i : i + size] for i in range(0, len(text), size)]


def create_pdf(text: str) -> bytes:
    pdf = FPDF()
    pdf.add_page()
    # Unicode Font
    font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    pdf.add_font("DejaVu", "", font_path, uni=True)
    pdf.set_font("DejaVu", size=12)
    pdf.set_auto_page_break(auto=True, margin=15)
    for line in text.split("\n"):
        pdf.multi_cell(0, 10, line)
    output = io.BytesIO()
    pdf.output(output)
    return output.getvalue()


uploaded_file = st.file_uploader("PDF-Datei auswählen", type=["pdf"])
target_lang = st.selectbox(
    "Zielsprache",
    [
        "Deutsch",
        "Englisch",
        "Französisch",
        "Spanisch",
        "Italienisch",
        "Arabisch",
        "Chinesisch",
        "Japanisch",
    ],
)

if st.button("Übersetzen"):
    if not api_key:
        st.error("Bitte API-Schlüssel eingeben.")
    elif uploaded_file is None:
        st.error("Bitte eine PDF-Datei hochladen.")
    else:
        client = OpenAI(api_key=api_key, base_url="https://api.302.ai/v1")
        raw_text = extract_text(uploaded_file)
        chunks = chunk_text(raw_text)
        translated_chunks = []
        progress = st.progress(0)
        for i, chunk in enumerate(chunks):
            translated = translate_text(client, chunk, target_lang)
            translated_chunks.append(translated)
            progress.progress((i + 1) / len(chunks))
        translated_text = "\n".join(translated_chunks)
        pdf_bytes = create_pdf(translated_text)
        st.success("Übersetzung abgeschlossen!")
        st.download_button(
            "Übersetztes PDF herunterladen",
            data=pdf_bytes,
            file_name="translation.pdf",
            mime="application/pdf",
        )
