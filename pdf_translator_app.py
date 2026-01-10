import io
from typing import List

import streamlit as st
from openai import OpenAI
import fitz  # PyMuPDF

st.set_page_config(page_title="PDF Übersetzer", page_icon="📄", layout="wide")

st.title("📄 PDF Übersetzer")
st.markdown(
    "Lade eine PDF-Datei hoch und übersetze sie in eine beliebige Zielsprache."
)

api_key = st.text_input("302.api Schlüssel", type="password")


def translate_text(client: OpenAI, text: str, target_language: str) -> str:
    prompt = f"Übersetze ins {target_language}:\n\n{text}"
    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[{"role": "user", "content": prompt}],
        temperature=0.2,
    )
    return response.choices[0].message.content.strip()


def chunk_text(text: str, size: int = 2000) -> List[str]:
    return [text[i : i + size] for i in range(0, len(text), size)]


def translate_pdf(file_bytes: bytes, client: OpenAI, target_language: str) -> bytes:
    doc = fitz.open(stream=file_bytes, filetype="pdf")
    out = fitz.open()
    progress = st.progress(0)
    for page_index in range(doc.page_count):
        src = doc[page_index]
        dst = out.new_page(width=src.rect.width, height=src.rect.height)

        # Bilder übernehmen
        for img in src.get_images(full=True):
            xref = img[0]
            for rect in src.get_image_rects(xref):
                pix = fitz.Pixmap(doc, xref)
                if pix.alpha:
                    pix = fitz.Pixmap(fitz.csRGB, pix)
                dst.insert_image(rect, pixmap=pix)

        # Textblöcke übersetzen und an gleicher Position einfügen
        for block in src.get_text("blocks"):
            x0, y0, x1, y1, text, block_no, block_type = block
            if block_type == 0 and text.strip():
                pieces = chunk_text(text)
                translated = []
                for piece in pieces:
                    try:
                        translated.append(translate_text(client, piece, target_language))
                    except Exception as exc:
                        raise RuntimeError(f"API-Fehler: {exc}")
                dst.insert_textbox(
                    fitz.Rect(x0, y0, x1, y1),
                    "\n".join(translated),
                    fontsize=11,
                    fontname="helv",
                )

        progress.progress((page_index + 1) / doc.page_count)

    return out.tobytes()


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
        try:
            client = OpenAI(api_key=api_key, base_url="https://api.302.ai/v1")
            pdf_bytes = uploaded_file.read()
            translated_pdf = translate_pdf(pdf_bytes, client, target_lang)
            st.success("Übersetzung abgeschlossen!")
            st.download_button(
                "Übersetztes PDF herunterladen",
                data=translated_pdf,
                file_name="translation.pdf",
                mime="application/pdf",
            )
        except Exception as exc:
            st.error(f"Fehler bei der Übersetzung: {exc}")
