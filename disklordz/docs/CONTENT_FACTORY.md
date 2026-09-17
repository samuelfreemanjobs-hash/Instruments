# Content Factory

One **product spec** is the source of truth for marketing and demos—not a separate manual copy pass.

## Outputs per product compile

```text
PRODUCT SPEC
 │
 ├── product_description.md
 ├── sales_page.html (or CMS blocks)
 ├── email_launch_sequence/
 ├── youtube_description.txt
 ├── short_form_scripts/
 ├── demo_video_brief.md
 ├── preset_descriptions.json
 ├── sound_demo_ideas.md
 ├── social_posts/
 └── artist_outreach_templates/
```

Triggered after QA pass, in parallel with packaging (compiler target `content_pack`).

## Audio Demo Generator

Critical for conversion—**“Hear what this does.”**

```text
Demo Generator
├── dry drum loop (from kit DNA)
├── processed loop (through plugin)
├── before/after A/B
├── beat + bass + melodic example
└── full mix stub
```

Inputs: asset IDs + plugin preset IDs + tempo/key from spec.  
Outputs: `demos/` on product page + YouTube upload metadata.

## Sonic problem framing (not generic genres)

Product Designer uses **problem statements**:

- DIRTY DIGITAL KICKS  
- DARK 808 ARCHITECTURE  
- 2000s DIGITAL DRUMS  
- MEMPHIS-STYLE DRUM TEXTURES  

Content Factory echoes the same language across copy and demos.

## Feedback loop

Customer feedback → **Feedback** table → Intelligence analysis → Product Improvement Requests → Orchestrator prioritizes next compile.

Example signal: recurring “mix knob” → P1 WO across affected plugin family.
