"""Local Aquila direction and implementation report; prior rounds remain intact."""
import importlib.util
import json
import sys
from html import escape
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
KIT = Path((ROOT / "reports/kit.path").read_text().strip())
spec = importlib.util.spec_from_file_location("page", KIT / "kit/page.py")
page = importlib.util.module_from_spec(spec)
spec.loader.exec_module(page)
decide = page.decide


def table(headers, rows):
    return ("<table><thead><tr>" + "".join(f"<th>{x}</th>" for x in headers)
            + "</tr></thead><tbody>" + "".join("<tr>" + "".join(f"<td>{x}</td>" for x in row)
                                                + "</tr>" for row in rows) + "</tbody></table>")


def main(output):
    out = Path(output)
    out.mkdir(parents=True, exist_ok=True)
    content = decide.recap(
        "Yes. Explore space from above, leave your battleship in orbit, then take a smaller craft down. "
        "My pick: a tilted 3D view with gentle depth. Generate nearby regions as you travel. "
        "Planets can wrap around without edges. Website and game design remain open.",
        [
            ("View", "My recommendation: a tilted 3D camera with gentle perspective."),
            ("Space", "Generate nearby regions and restore familiar places when revisited."),
            ("Landings", "Keep the large ship in orbit; descend in a smaller craft."),
            ("Planets", "Huge, finite worlds can wrap around without travel barriers."),
            ("Website", "Lead with exploration, simplify left navigation and keep the ship catalog concept."),
        ],
        {
            "did": "Read current page and game source, then developed a proposed direction.",
            "why": "The owner wants to discuss remaking both website and game around endless exploration.",
            "working": "Current source contains the quoted introduction, ship catalog and isometric game code.",
            "not_working": "No remake exists yet. Camera feel, streaming, landings and browser performance remain untested.",
            "next": "Discuss freedom of flight, then visual direction. No implementation has started.",
        },
    )
    body = "<p>Yes. The exploration idea makes sense, including leaving the battleship in orbit.</p>"
    body += "<p>My recommendation: build 3D worlds in <code>Three.js</code>, viewed diagonally from above with gentle perspective.</p>"
    body += "<p>This keeps the isometric feel while giving planet approaches a stronger sense of depth.</p>"
    body += table(["Camera", "Effect"], [
        ("Strict isometric", "Parallel projection keeps equal objects the same size at different distances. Clear shapes; weaker distance cues."),
        ("Tilted perspective", "Nearby objects appear larger. My pick for the sense of scale and discovery you described."),
    ])
    body += '<p>Camera facts: <a href="https://threejs.org/docs/pages/OrthographicCamera.html">Orthographic camera</a> and <a href="https://threejs.org/docs/pages/PerspectiveCamera.html">perspective camera</a>. The creative choice is my recommendation.</p>'
    body += "<p>Endless travel comes from generating nearby regions and releasing distant ones. Returning should recreate the same region and saved changes.</p>"
    body += "<p>That means practically endless exploration within finite computing and storage limits. It does not promise a browser session lasts forever.</p>"
    body += "<p>A planet can have no travel boundary while remaining finite. Keep flying and eventually circle back.</p>"
    body += table(["Travel stage", "Proposed experience"], [
        ("Space", "Pilot the large ship among planets, nebulae and discoveries."),
        ("Orbit", "Choose a landing region and launch the smaller craft."),
        ("Descent", "Use a short transition into the surface view for the first version."),
        ("Surface", "Explore terrain from the same tilted viewpoint, then return to the orbiting ship."),
    ])
    body += "<p>For the website: lead with that journey, keep left navigation compact, and develop the ships page into a hangar catalog.</p>"
    body += "<p>Flight controls, planet scale, art style and website layout remain proposals. Your description has not settled those choices.</p>"
    body += "<p>The rendering library supplies 3D graphics. We still need movement, collisions, world generation and saving.</p>"
    body += '<p>Scope reference: <a href="https://threejs.org/manual/pages/game.html">official game guide</a>. Resource reference: <a href="https://threejs.org/manual/pages/cleanup.html">official cleanup guide</a>.</p>'
    body += "<p>Evidence: source review and official documentation. No live gameplay or remake performance tested.</p>"
    body += "<p>Shared knowledge search returned no usable guidance in the inspected results; one query exhausted its service budget.</p>"
    body += "<p>No course material copied. No game source, dependencies, shared staging, deployment or paid generation changed.</p>"
    content += decide.info_thread("aquila-direction-r1-info", "An isometric universe fits the exploration idea.", body_html=body, short="Proposed direction")
    qid = "aquila-direction-r1-flight"
    choices = table(["Flight", "Tradeoff", "Choose"], [
        ("Mostly across a plane", "My starting recommendation. Easier to judge ships and destinations; height still matters during descent.",
         decide.pick(qid, "plane", "Mostly across a plane", mode="single", mine=True)),
        ("Freely in every direction", "Fly above and below other ships. More freedom; judging depth and steering needs more care.",
         decide.pick(qid, "free", "Freely in every direction", mode="single")),
        ("Still exploring", "Keep discussing the feel before choosing controls.",
         decide.pick(qid, "open", "Still exploring", mode="single")),
    ])
    content += decide.question(qid,
        "Should ships fly mostly across a plane, or move freely in all three dimensions?",
        [], short="Freedom of flight", anchors_html=choices,
        why="This shapes camera behavior and controls. A preference here does not authorize implementation.")
    content += decide.whats_next("aquila-direction-r1-next",
        "Continue design discussion after your flight preference; settle the website look before implementation.", short="Design discussion")
    rounds = page.entry(1, "Endless exploration", content, when="2026-11-15 UTC")
    flight = decide.recap(
        "Full 3D flight chosen. You can pass over or under other ships. "
        "My camera pick now: follow behind your ship, with an angled overview available. "
        "An elevated view can still work. Camera choice remains yours; no remake has started.",
        [
            ("Chosen", "Ships move freely in all three dimensions."),
            ("Camera", "My recommendation now: follow behind and slightly above the ship."),
            ("Overview", "An angled overview remains useful for navigation and seeing nearby ships."),
            ("Alternative", "An elevated camera can support full 3D flight, with rotation and clear height cues."),
            ("Scope", "Flight direction saved. Camera choice and implementation scope remain open."),
        ],
        {
            "did": "Recorded your flight choice and updated the camera recommendation.",
            "why": "Free vertical movement changes how players judge direction and distance.",
            "working": "Your movement preference is settled and saved in project records.",
            "not_working": "Camera feel and controls remain untested; no game changes made.",
            "next": "Choose the default flight view, then continue design discussion.",
        },
    )
    flight += decide.reply(
        "aquila-direction-r2-flight-answer", "aquila-direction-r1-flight", "Freedom of flight",
        "Yeah, freely above and below other ships.",
        "<p>Full 3D movement selected. Recorded from your chat answer; no report submission created.</p>",
        short="Your flight choice",
    )
    flight += decide.info_thread(
        "aquila-direction-r2-info", "Free 3D movement confirmed; camera recommendation updated.",
        body_html="<p>I now favor a camera behind and slightly above the ship, looking along its travel direction.</p>"
                  "<p>My aim: make climbing, diving and approaching planets easier to read. An angled overview could remain available.</p>"
                  "<p>Full 3D movement still works with an elevated view. Camera rotation and height cues would need careful design.</p>"
                  "<p>This choice does not settle ship roll, flight realism or the default camera. Those remain open.</p>"
                  "<p>Project records now reflect your answer. No game source changed; no flight or camera behavior tested.</p>",
        short="Camera implications",
    )
    camera_qid = "aquila-direction-r2-camera"
    camera_choices = table(["Default view", "Feel", "Choose"], [
        ("Follow behind the ship", "My recommendation for piloting. See the ship and the space it is heading toward.",
         decide.pick(camera_qid, "chase", "Follow behind the ship", mode="single", mine=True)),
        ("Stay at an elevated angle", "Keep the original visual feel. Orbit the camera when needed to judge depth.",
         decide.pick(camera_qid, "elevated", "Stay at an elevated angle", mode="single")),
    ])
    flight += decide.question(camera_qid,
        "Should the main flight view trail your ship or stay at an elevated angle?", [],
        short="Default flight view", anchors_html=camera_choices,
        why="Both support your flight choice. This chooses the normal view, not whether other views can exist.")
    flight += decide.whats_next("aquila-direction-r2-next",
        "Continue camera discussion after your preference; website look remains open.",
        short="Camera direction", follows="aquila-direction-r1-next")
    rounds = page.entry(2, "Free flight", flight, when="2026-11-15 UTC") + rounds
    chosen = decide.recap(
        "Your reply came through: follow behind the ship. "
        "That is now the default camera, alongside free 3D movement. "
        "Both choices are saved. Next discussion: the look of the game and website. No remake has started.",
        [
            ("Received", "Read your submitted camera selection."),
            ("Camera", "Follow behind the ship is the chosen default."),
            ("Movement", "Free flight in all three dimensions remains settled."),
            ("Scope", "Project notes updated; game source unchanged."),
            ("Next", "Discuss the look of the universe and website."),
        ],
        {
            "did": "Read your report reply and saved the camera decision.",
            "why": "The design records must reflect your selected view.",
            "working": "Your submitted choice is recorded; the old camera question is closed.",
            "not_working": "Camera tuning and game runtime remain untested.",
            "next": "Continue visual direction discussion; no new decision required in this reply.",
        },
    )
    chosen += decide.info_thread(
        "aquila-direction-r3-info", "Your submitted camera choice is saved.",
        body_html='<p>Your selection: <q>Follow behind the ship</q>.</p>'
                  "<p>This sets the default flight view. Free movement above and below other ships remains selected.</p>"
                  "<p>Exact camera distance, height and optional views remain design details. No game source changed.</p>"
                  "<p>Evidence: your real report submission. Earlier rounds and the submitted answer remain in history.</p>",
        short="Camera chosen",
    )
    chosen += decide.whats_next("aquila-direction-r3-next",
        "Next discussion: visual direction for the universe and website; no new answer needed now.",
        short="Visual direction", follows="aquila-direction-r2-next")
    rounds = page.entry(3, "Camera chosen", chosen, when="2026-11-15 UTC") + rounds
    glass = decide.recap(
        "Yes. The universe becomes the website, with glass controls over it. "
        "Robinhood's prototype already uses a full-screen scene and floating controls. "
        "For Aquila, Home, Ships and Play could share that world. This remains design discussion.",
        [
            ("Reference", "Read Robinhood's current prototype source and saved website proposal."),
            ("World", "The 3D scene fills the screen and stays visible while panels open."),
            ("Interface", "Navigation, ship details and settings float in glass panels."),
            ("Views", "Home, Ships and Play could share one persistent scene."),
            ("Scope", "Reference checked and proposal saved. Neither application's source changed."),
        ],
        {
            "did": "Checked the Robinhood reference and mapped the same interface approach to Aquila.",
            "why": "You want the 3D world to be the main experience, with glass controls layered over it.",
            "working": "Reference source confirms a full-screen scene, floating controls and glass-styled settings.",
            "not_working": "The full reference website remains proposed. Aquila's new interface is not built or tested.",
            "next": "Continue discussing panel layout and appearance within this approach.",
        },
    )
    glass_body = "<p>Yes. The universe becomes the website, with glass controls layered over it.</p>"
    glass_body += "<p>I checked Robinhood's current prototype source and the saved website proposal.</p>"
    glass_body += "<p>The scene fills the viewport. Settings use tinted, blurred panels; hover details are transparent text over the scene.</p>"
    glass_body += "<p>The broader website shell remains proposed. Your message supplies the newest glass-overlay direction.</p>"
    glass_body += table(["Aquila view", "Proposed behavior"], [
        ("Home", "The universe is already visible. Introductory text and entry controls float over it."),
        ("Navigation", "A compact glass strip opens panels while keeping the world visible."),
        ("Ships", "The camera could enter a hangar view, with ship selection and specifications in glass panels."),
        ("Play", "The experience switches to the chosen chase camera, with a small flight display."),
        ("Settings", "An overlay opens over the scene and closes back into the same experience."),
    ])
    glass_body += "<p><code>Three.js</code> renders the world. Ordinary HTML and CSS provide the interface above its canvas.</p>"
    glass_body += "<p>I recommend tinted translucency, restrained blur and clear text, keeping details readable against bright nebulae.</p>"
    glass_body += "<p>Panel input should stay with the panel, so a menu click does not also steer or fire the ship.</p>"
    glass_body += "<p>Those are Aquila proposals. Exact layout, art and interactions remain open.</p>"
    glass_body += "<p>Source inspection only. No reference runtime launched, assets copied or application source changed.</p>"
    glass_body += "<p>Reference locations: <code>C:/RobinHoodTrading/portal/capital-orbit/demo.js</code> and <code>style.css</code>.</p>"
    glass_body += "<p>Saved website proposal: round 85 in <code>C:/RobinHoodTrading/reports/crypto-gap-scanner-2026-11-11/build_page.py</code>.</p>"
    glass += decide.info_thread("aquila-direction-r4-info", "The glass-overlay approach fits Aquila.",
        body_html=glass_body, short="One world, glass controls")
    glass += decide.whats_next("aquila-direction-r4-next",
        "Continue interface design discussion; exact glass styling and panel layout remain open.",
        short="Interface direction", follows="aquila-direction-r3-next")
    rounds = page.entry(4, "The universe as the website", glass, when="2026-11-15 UTC") + rounds
    built = decide.recap(
        "Built the first playable Aquila. The universe stays behind glass navigation, fleet choices and settings. "
        "Fly freely, visit planets by shuttle, and resume your saved voyage. Local build and browser checks pass. "
        "This is an exploration prototype; detailed gameplay comes next.",
        [
            ("World", "One persistent 3D scene connects the website and game."),
            ("Interface", "Home, Fleet, Atlas, Play and Settings use glass overlays."),
            ("Flight", "Three ships support free movement and the chosen chase camera."),
            ("Planets", "Arrival assist reaches orbit; a smaller shuttle explores the surface and returns to the carrier."),
            ("Checks", "Production build, ten browser check groups, four model checks and six safety tests pass."),
        ],
        {
            "did": "Built the first local website and game slice after your approval.",
            "why": "You chose a living universe with glass controls, free flight and a camera behind the ship.",
            "working": "Navigation, ship selection, flight, sector travel, saves, surface visits and graphics recovery passed browser checks.",
            "not_working": "Full game systems and seamless descent remain unfinished. Physical-phone performance and desktop packaging remain untested.",
            "next": "Review the local experience and choose the next art or gameplay changes.",
        },
    )
    built_body = '<p>First playable version built. <a href="http://127.0.0.1:4200/home">Open the local Aquila preview</a>.</p>'
    built_body += '<p>Your approval: <q>Yep, let\'s do that.</q> The build follows the shared-world and glass-interface proposal.</p>'
    built_body += table(["Area", "Available now"], [
        ("Website", "A live universe behind Home, Fleet, Atlas and Settings. Navigation keeps the same renderer alive."),
        ("Ships", "Three modeled ships with distinct handling, speed and proportions."),
        ("Flight", "Forward, reverse, sideways and vertical movement; steering, boost, brake, pause and a chase camera."),
        ("Exploration", "Nearby sectors load as you travel. Returning recreates the same worlds. Distant resources are released."),
        ("Planet visits", "Approach orbit, launch a smaller shuttle, explore curved terrain, then return to your carrier."),
        ("Saving", "Ship, orbital position and preferences save on this device. Reloading a surface visit returns to orbit."),
    ])
    built_body += '<p><code>Three.js</code> renders original procedural meshes and shaders. <code>Angular</code> remains the application shell.</p>'
    built_body += "<p>No Robinhood code, media or financial data copied. No paid generation, deployment, commit or push performed.</p>"
    built_body += "<p>Ten browser check groups pass with no unexpected page or console errors. Four model checks and six safety tests pass.</p>"
    built_body += "<p>Production compilation passes. Rendered checks used an isolated Chrome session, including desktop and phone-sized layouts.</p>"
    built_body += "<p>Phone-sized layouts do not prove physical-phone performance. Long-duration travel and the packaged desktop app remain untested.</p>"
    built_body += "<p>Surface terrain uses a separate sphere. Seamless descent, detailed geography, combat, progression and multiplayer are not implemented.</p>"
    built_body += "<p>Paseo's browser host is disconnected. The preview runs locally; I could not open it inside your workspace.</p>"
    built_body += "<p>The following views come from the running application.</p>"
    built_body += page.figure(page.copy_media(HERE / "media/home-desktop.png", out), "5.1 Home",
                              " The universe fills the page. Navigation and entry controls stay above it.", look=True)
    built_body += page.figure(page.copy_media(HERE / "media/fleet-desktop.png", out), "5.2 Fleet",
                              " Select a ship while its model remains in the live scene.")
    built_body += page.figure(page.copy_media(HERE / "media/flight-desktop.png", out), "5.3 Flight",
                              " Free flight with the camera behind the ship and small glass instruments.")
    built_body += page.figure(page.copy_media(HERE / "media/surface-desktop.png", out), "5.4 Surface",
                              " The survey shuttle flies over a procedural globe. The carrier remains in orbit.")
    built_body += page.grid(
        page.figure(page.copy_media(HERE / "media/home-mobile.png", out), "5.5 Phone home", " Narrow layout with navigation below the scene."),
        page.figure(page.copy_media(HERE / "media/fleet-mobile.png", out), "5.6 Phone fleet", " Ship selection and details remain within the viewport."),
        page.figure(page.copy_media(HERE / "media/flight-mobile.png", out), "5.7 Phone flight", " Pointer controls provide thrust, steering and height."),
    )
    built += decide.info_thread("aquila-direction-r5-info", "The first playable universe and glass interface are ready locally.",
        body_html=built_body, short="Local playable build")
    built += decide.whats_next("aquila-direction-r5-next",
        "The local preview is ready for review. Further art and gameplay changes can follow your feedback; deployment requires separate authority.",
        short="Review the first version", follows="aquila-direction-r4-next")
    rounds = page.entry(5, "A playable universe", built, when="2026-11-15 UTC") + rounds
    refined = decide.recap(
        "Glass panels are nearly clear, with brighter text. Ships have metal detail; planets have richer terrain and separate clouds. "
        "Hold the right mouse button and drag over the flight canvas to steer. Browser menus still work on the interface.",
        [
            ("Glass", "Navigation, panels and buttons use faint fills and light blur."),
            ("Text", "Brighter lettering and soft shadows keep controls readable over the world."),
            ("Ships", "Hull seams, scuffs, inspection details and reflections add surface detail."),
            ("Planets", "Continents, coasts, terrain shading, ocean highlights and separate clouds add depth."),
            ("Mouse", "Right-drag steers during flight; release stops steering. Interface menus keep their normal behavior."),
        ],
        {
            "did": "Refined glass, text, ships and planets; added right-drag flight controls.",
            "why": "You want the universe to show through almost invisible controls, with richer surfaces and game mouse input.",
            "working": "Production compilation and eleven browser check groups pass. Desktop and phone layouts were inspected.",
            "not_working": "Physical-phone performance and other browser engines remain untested. Surface exploration still uses simplified terrain.",
            "next": "The local preview remains ready for your review.",
        },
    )
    refined_body = '<p>Nearly clear glass, richer surfaces and right-drag steering are ready in the <a href="http://127.0.0.1:4200/home">local preview</a>.</p>'
    refined_body += table(["Area", "Change"], [
        ("Glass", "Faint fills and light blur across navigation, panels, buttons, settings and touch controls, including pause and help."),
        ("Readability", "Brighter text, soft shadows and faint edges keep labels visible as the world changes behind them."),
        ("Ships", "Metal plates, finish variation, scuffs, softer bevels, inspection hatches, fasteners, engine rims and reflective canopies."),
        ("Planets", "Detailed continents, shallow coasts, relief shading, ocean highlights and a separate cloud shell."),
        ("Controls", "Hold the right mouse button and drag over open flight space. Left drag and touch drag still work."),
    ])
    refined_body += "<p>Mouse release and pause stop steering. Browser menus remain available on interface controls and outside the flight view.</p>"
    refined_body += "<p>These surfaces use original procedural art. No downloaded ship models, purchased textures or paid generation were needed.</p>"
    refined_body += "<p>Production compilation passes. Eleven browser check groups pass without unexpected console or page errors.</p>"
    refined_body += "<p>Checks include right-drag steering, release, pause, surface flight, menu scope, navigation, saving, sector travel and graphics recovery.</p>"
    refined_body += "<p>Two intermediate checks needed timing fixes: await route content and observed slowdown instead of an immediate count or fixed delay.</p>"
    refined_body += "<p>Rendered checks used isolated Chrome at desktop and phone sizes. Physical phones and other browser engines remain untested.</p>"
    refined_body += "<p>The workspace preview opened once, then its browser host disconnected. The local preview server remains running.</p>"
    refined_body += '<p>Material reference: <a href="https://threejs.org/docs/pages/MeshStandardMaterial.html">official standard material guide</a>.</p>'
    refined_body += '<p>Firefox can show its menu with Shift and right-click. Reference: <a href="https://developer.mozilla.org/en-US/docs/Web/API/Element/contextmenu_event">browser menu behavior</a>.</p>'
    refined_body += page.figure(page.copy_media(HERE / "media/r6-home-desktop.png", out), "6.1 Home",
                                " Clear entry controls and a more detailed world.", look=True)
    refined_body += page.figure(page.copy_media(HERE / "media/r6-fleet-desktop.png", out), "6.2 Fleet",
                                " Surface detail and softer metal edges on the ship.")
    refined_body += page.figure(page.copy_media(HERE / "media/r6-help-desktop.png", out), "6.3 Controls",
                                " The world remains visible through the controls overlay.")
    refined_body += page.figure(page.copy_media(HERE / "media/r6-surface-desktop.png", out), "6.4 Surface",
                                " Text remains readable over bright terrain with nearly clear instruments.")
    refined_body += page.grid(
        page.figure(page.copy_media(HERE / "media/r6-fleet-mobile.png", out), "6.5 Phone fleet", " Clear ship choices and details."),
        page.figure(page.copy_media(HERE / "media/r6-settings-mobile.png", out), "6.6 Phone settings", " The same glass treatment carries into settings."),
        page.figure(page.copy_media(HERE / "media/r6-flight-mobile.png", out), "6.7 Phone flight", " Touch controls retain faint fills and visible labels."),
    )
    refined += decide.info_thread("aquila-direction-r6-info", "Glass is nearly clear; ships and planets have richer surfaces.",
        body_html=refined_body, short="Glass, surfaces and mouse controls")
    refined += decide.whats_next("aquila-direction-r6-next",
        "Local preview ready for review. Further visual changes can follow your feedback; deployment still requires separate authority.",
        short="Review the refinements", follows="aquila-direction-r5-next")
    rounds = page.entry(6, "Clear glass and richer worlds", refined, when="2026-11-15 UTC") + rounds
    quality_fix = decide.recap(
        "Quality now has three direct choices: Light, Balanced and High. No dropdown popup. "
        "Changing quality resizes the graphics once, only when needed. Clicks, touch, keyboard selection and saved settings pass checks.",
        [
            ("Control", "Three visible quality choices replace the popup menu."),
            ("Graphics", "Changed quality resizes the graphics once; unchanged sizes skip that work."),
            ("Other settings", "Motion and steering changes no longer reset the graphics buffer."),
            ("Input", "Pointer, touch and arrow keys select quality, and the choice survives reload."),
            ("Checks", "Production build, four focused checks and eleven game check groups pass."),
        ],
        {
            "did": "Replaced the quality popup and removed unnecessary graphics resizing.",
            "why": "You reported the dropdown hanging when changing quality.",
            "working": "Direct choices respond to clicks, touch and keyboard input; rendering continues and preferences save.",
            "not_working": "The original lockup was not reproduced. Its exact browser cause remains unconfirmed.",
            "next": "Refresh settings to load the new quality choices.",
        },
    )
    quality_body = '<p>Quality now uses three direct choices in <a href="http://127.0.0.1:4200/settings">the local settings view</a>.</p>'
    quality_body += "<p>Light, Balanced and High remain visible, with the selected choice marked. No popup opens.</p>"
    quality_body += "<p>Source inspection found two graphics-buffer resets per settings update, including unrelated motion and steering changes.</p>"
    quality_body += "<p>Quality changes now resize once when dimensions differ. Unrelated preferences and repeated selections leave the buffer alone.</p>"
    quality_body += "<p>The old popup responded during my workspace check. I could not confirm the exact cause of your original hang.</p>"
    quality_body += "<p>The replacement passes pointer, touch, keyboard, save and continued-rendering checks, including a dense display.</p>"
    quality_body += "<p>Production build, four focused quality checks and eleven game check groups pass. No unexpected browser errors.</p>"
    quality_body += "<p>The workspace browser reconnected; updated settings and this report opened. Full checks used isolated Chrome.</p>"
    quality_body += "<p>Physical phones and other browser engines remain untested.</p>"
    quality_body += page.figure(page.copy_media(HERE / "media/r7-quality-desktop.png", out), "7.1 Quality choices",
                                " All quality levels stay visible. Keyboard focus remains clear.", look=True)
    quality_body += page.figure(page.copy_media(HERE / "media/r7-quality-mobile.png", out), "7.2 Touch choices",
                                " Direct touch targets replace the narrow dropdown.")
    quality_fix += decide.info_thread("aquila-direction-r7-info", "Quality selection now uses three direct controls.",
        body_html=quality_body, short="Quality control fix")
    quality_fix += decide.whats_next("aquila-direction-r7-next",
        "Refresh the local settings view to load the new controls. The preview remains running.",
        short="Use the updated settings", follows="aquila-direction-r6-next")
    rounds = page.entry(7, "Direct quality controls", quality_fix, when="2026-11-15 UTC") + rounds
    scale_discussion = decide.recap(
        "Yes. Planets can feel huge without copying real planetary sizes. Fix the ship-to-planet proportions, keep travel fast, "
        "and reveal detailed terrain during shuttle descent. The nearby chase camera keeps your ship visible. This remains a proposal; game unchanged.",
        [
            ("Current scale", "The starting planet spans roughly nine ship lengths, which explains the toy-like proportions."),
            ("Size", "Shrinking ships or enlarging planets changes the ratio; shrinking everything equally does not."),
            ("Camera", "Your ship can remain large on screen because the chase camera stays nearby."),
            ("Worlds", "Use distant globes, huge orbital horizons and nearby terrain detail, with faster travel between worlds."),
            ("Scope", "Discussion only. No game code or scale values changed."),
        ],
        {
            "did": "Checked current ship, planet, camera and approach sizes; proposed a scale direction.",
            "why": "You noticed ships look too large beside planets and asked about scale tricks.",
            "working": "The prototype already separates orbital and shuttle views and renders space relative to the ship.",
            "not_working": "Current proportions remain small. A convincing large-planet approach has not been built or tested.",
            "next": "Continue the scale discussion before changing the game.",
        },
    )
    scale_body = "<p>My recommendation: much larger planet-to-ship proportions, compressed travel times, and terrain detail concentrated near the shuttle.</p>"
    scale_body += "<p>The starting planet measures 570 world units across. The starting ship's central hull measures 60, excluding engines and bevels.</p>"
    scale_body += "<p>That is 9.5 central hull lengths across. Current proportions explain why approaching the globe feels like approaching a small object.</p>"
    scale_body += "<p>Your scaling idea works: shrinking only the ship tenfold changes the ratio like enlarging only the planet tenfold.</p>"
    scale_body += "<p>Camera distances, speeds and collision clearances must follow the chosen scale. Smaller coordinate numbers alone do not reduce rendering work.</p>"
    scale_body += "<p>Earth-sized worlds are technically possible, but need careful precision and terrain streaming. Exact real-world dimensions are not required here.</p>"
    scale_body += "<p>A larger sphere can keep the same triangle count. Detailed surfaces and screen coverage determine additional work.</p>"
    scale_body += table(["Stage", "Proposed experience"], [
        ("Distant space", "A complete globe with modest detail. Faster cruise keeps travel practical."),
        ("Approach", "The globe grows beyond the frame. Broad cloud layers, a shallow horizon and slow terrain motion imply enormous scale."),
        ("Orbit", "The main ship stops at a clear orbital limit. The chase camera keeps it visible against the planet."),
        ("Shuttle descent", "A transition reveals detailed local terrain while distant land stays simpler. Curvature and landmarks should remain consistent."),
        ("Surface travel", "Mountains, cloud height and ground features establish size. Load nearby terrain as the shuttle travels."),
    ])
    scale_body += "<p>Your ship need not become a dot in normal flight. Nearby objects remain prominent even beside enormous distant ones.</p>"
    scale_body += "<p>From a distant planet-wide view, the ship should become tiny. Camera placement cannot fix incorrect physical proportions by itself.</p>"
    scale_body += "<p>Source inspection supports the current dimensions. No scale prototype, performance test or new rendered comparison ran this turn.</p>"
    scale_body += '<p>Engine references: <a href="https://threejs.org/docs/pages/SphereGeometry.html">sphere radius and geometry detail</a>, '
    scale_body += '<a href="https://threejs.org/docs/pages/LOD.html">distance-based detail</a>, and '
    scale_body += '<a href="https://threejs.org/manual/pages/cameras.html">perspective and depth precision</a>.</p>'
    scale_discussion += decide.info_thread("aquila-direction-r8-info", "Convincing planetary scale is possible without literal real-world dimensions.",
        body_html=scale_body, short="Planet scale discussion")
    scale_discussion += decide.whats_next("aquila-direction-r8-next",
        "Keep discussing the proposed scale and approach experience. Implementation waits for your direction; existing preview remains unchanged.",
        short="Discuss scale before changes", follows="aquila-direction-r7-next")
    rounds = page.entry(8, "Making planets feel immense", scale_discussion, when="2026-11-15 UTC") + rounds
    performance = decide.recap(
        "Yes. Browser performance must shape the design. Keep distant worlds simple and detailed terrain near the player. "
        "Limit loaded resources, screen resolution and effects. Test one planet approach on ordinary hardware before expanding. "
        "Current checks prove behavior, not broad device performance.",
        [
            ("Cost", "Visible detail, pixels, effects, memory and terrain generation matter more than numerical planet diameter."),
            ("Terrain", "Use simple distant worlds and detailed nearby patches, with bounded loading and cleanup."),
            ("Smoothness", "Prepare terrain gradually so arrivals do not stall controls."),
            ("Devices", "Measure frame times and memory on representative laptops and phones before promising performance."),
            ("Status", "Discussion only. No game changes or new performance measurements."),
        ],
        {
            "did": "Recorded browser performance as a scale-design constraint and checked the engine guidance.",
            "why": "You raised performance concerns about massive planets running in the browser.",
            "working": "The prototype already bounds loaded space sectors and releases their graphics resources.",
            "not_working": "Detailed planetary streaming and performance on representative laptops or phones remain untested.",
            "next": "Keep the design within a measured device budget; a focused approach benchmark remains proposed.",
        },
    )
    performance_body = "<p>Yes. The browser can use the graphics card, but we still need strict limits on work and memory.</p>"
    performance_body += "<p>Large coordinates alone do not make a planet expensive. Filling the screen with complex terrain or clouds can.</p>"
    performance_body += table(["Budget", "Proposed control"], [
        ("Geometry", "Simple distant globes; detailed terrain only near the shuttle. Keep active terrain patches bounded."),
        ("Memory", "Load nearby resources and release distant ones. Hidden objects can still occupy memory."),
        ("Pixels and effects", "Cap render resolution and cloud, shadow and reflection complexity for each quality level."),
        ("Generation", "Prepare terrain in small batches or background workers; avoid long work on the input thread."),
        ("Validation", "Measure approach, descent and sustained travel on representative hardware, including frame-time spikes and memory growth."),
    ])
    performance_body += "<p>Those are design proposals, not implemented terrain optimizations or promised frame rates.</p>"
    performance_body += "<p>Current space streaming and resolution choices are useful foundations. Surface terrain still uses one separate, fixed-detail sphere.</p>"
    performance_body += "<p>Existing browser checks used a high-end graphics card. Phone-sized layouts do not establish physical-phone speed, heat or memory behavior.</p>"
    performance_body += "<p>My recommended next implementation step, once requested: one convincing planet approach with a measured performance budget.</p>"
    performance_body += '<p>References: <a href="https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API">browser graphics acceleration</a>, '
    performance_body += '<a href="https://threejs.org/docs/pages/LOD.html">distance-based detail</a>, '
    performance_body += '<a href="https://threejs.org/manual/pages/responsive.html">render resolution</a>, and '
    performance_body += '<a href="https://threejs.org/manual/pages/cleanup.html">graphics resource cleanup</a>.</p>'
    performance += decide.info_thread("aquila-direction-r9-info", "Browser performance must shape planetary detail and loading.",
        body_html=performance_body, short="Browser performance limits")
    performance += decide.whats_next("aquila-direction-r9-next",
        "Continue scale and performance discussion. Game changes and benchmarking wait for your direction; no new action is required now.",
        short="Keep performance measurable", follows="aquila-direction-r8-next")
    rounds = page.entry(9, "Massive worlds within browser limits", performance, when="2026-11-15 UTC") + rounds
    scaled = decide.recap(
        "Planets now dwarf your ship. Fast cruise keeps travel practical, and a shuttle descends onto the same world's terrain. "
        "Nearby detail loads gradually within fixed limits. Local checks pass; real laptop and phone performance remains untested.",
        [
            ("Scale", "The starting planet spans 2,000 central hull lengths; the nearby chase camera keeps your ship readable."),
            ("Travel", "Deep-space cruise is faster, arrival routes avoid planets, and main ships stop at orbital limits."),
            ("Descent", "The shuttle descends onto shared geography, with detailed terrain concentrated nearby."),
            ("Budgets", "Quality limits terrain, screen pixels, clouds, stars and shading; old terrain releases its buffers."),
            ("Proof", "Build and local checks pass. Throttled testing still shows brief spikes; physical-device performance remains unverified."),
        ],
        {
            "did": "Built larger planetary proportions, nearby terrain loading, shuttle descent and graphics budgets.",
            "why": "You approved the scale and browser-performance recommendations.",
            "working": "Approach, descent, moving terrain, flight controls, saves, quality changes and both depth modes pass local checks.",
            "not_working": "Physical devices remain untested. Brief throttled frame spikes and coarse distant silhouettes remain.",
            "next": "Local preview ready. Actual laptop and phone testing remains the next performance check.",
        },
    )
    scale_built_body = '<p>Larger worlds and bounded terrain are ready in the <a href="http://127.0.0.1:4200/home">local preview</a>.</p>'
    scale_built_body += table(["Area", "Built behavior"], [
        ("Proportions", "Vesper spans 120 km, against a 60 m central ship hull. These are game proportions, not real astronomical dimensions."),
        ("Flight", "Fast cruise slows near worlds. Swept collision keeps carriers outside terrain, including boosted approaches."),
        ("Arrival", "Assisted routes bend around blocking planets. Manual input still cancels the course."),
        ("Descent", "A four-second shuttle descent uses the same planet radius, coastline and height field. Reduced motion skips the animation."),
        ("Terrain", "Nearby patches refine gradually. Coarse coverage remains while detail arrives; old patches release their graphics buffers."),
        ("Quality", "Light, Balanced and High now control terrain detail, pixel budgets and effects together."),
        ("Saves", "Old voyages migrate into the enlarged sectors while retaining ship, rotation and sector identity."),
    ])
    scale_built_body += "<p>Production compilation, nine model tests, six safety tests, eleven game groups and four quality groups pass.</p>"
    scale_built_body += "<p>Seven scale groups also pass, including both depth modes, carrier collision, terrain cleanup and quality budgets.</p>"
    scale_built_body += "<p>Measurements used isolated Chrome on this desktop's RTX 5090, with the development preview.</p>"
    scale_built_body += "<p>The constrained case uses a phone viewport and fourfold CPU throttling on the same desktop graphics card.</p>"
    scale_built_body += "<p>Frame times below are browser intervals, not isolated graphics-card timings. Lower values are better.</p>"
    scale_built_body += table(["Case", "95th percentile", "Longest frame"], [
        ("Desktop: first approach", "4.8 ms", "12.7 ms"),
        ("Desktop: descent", "4.8 ms", "12.8 ms"),
        ("Desktop: 30-second surface flight", "4.9 ms", "33.3 ms"),
        ("Constrained: first approach", "26.0 ms", "158.5 ms"),
        ("Constrained: descent", "16.6 ms", "45.7 ms"),
        ("Constrained: 30-second surface flight", "12.6 ms", "76.1 ms"),
        ("Constrained: warmed approach", "12.7 ms", "20.9 ms"),
    ])
    scale_built_body += "<p>Terrain buffers stay below 0.71, 1.20 and 1.85 MB across Light, Balanced and High. Browser and driver overhead are excluded.</p>"
    scale_built_body += "<p>Balanced terrain held at most 118 patches while creating 290 and releasing 172 during the measured excursion.</p>"
    scale_built_body += "<p>A first-descent shader stall was fixed. Remaining throttled spikes need physical-device testing before broad performance claims.</p>"
    scale_built_body += "<p>Thirty-second runs do not prove long-session memory stability. Real laptops, phones, other browser engines and packaged desktop runtime remain untested.</p>"
    scale_built_body += "<p>Distant hills and ring edges remain coarse. Descent remains a controlled craft transition; walking and ground landing are not implemented.</p>"
    scale_built_body += page.figure(page.copy_media(HERE / "media/r10-home-desktop.png", out), "10.1 Larger worlds",
                                    " A complete globe from a distance, with the glass interface above the live scene.")
    scale_built_body += page.figure(page.copy_media(HERE / "media/r10-planet-horizon.png", out), "10.2 Orbital horizon",
                                    " At 5.2 km above sea level, the world fills the view while the ship stays readable.", look=True)
    scale_built_body += page.figure(page.copy_media(HERE / "media/r10-desktop-balanced-surface.png", out), "10.3 Shuttle survey",
                                    " Shared terrain below the smaller craft, 350 m above ground.")
    scale_built_body += page.figure(page.copy_media(HERE / "media/r10-phone-viewport-cpu4-light-surface.png", out), "10.4 Narrow viewport",
                                    " Light quality with touch controls. This desktop capture does not prove physical-phone performance.")
    scaled += decide.info_thread("aquila-direction-r10-info", "Larger worlds, shared terrain and browser budgets are implemented locally.",
        body_html=scale_built_body, short="Scale and performance implementation")
    scaled += decide.whats_next("aquila-direction-r10-next",
        "Local preview ready for review. Physical laptop and phone testing remains open; those devices are unavailable here.",
        short="Review the larger worlds", follows="aquila-direction-r9-next")
    rounds = page.entry(10, "Larger worlds, bounded detail", scaled, when="2026-11-15 UTC") + rounds
    battleship = decide.recap(
        "Feasible. The small craft become shuttles; a giant research battleship becomes your home and main ship. "
        "Walk its bridge, use consoles, and switch outside while controlling the same vessel. "
        "Start with a bridge, hangar and one working combat encounter; expand the ship from there.",
        [
            ("Roles", "Existing small craft become shuttles. The new battleship serves as home, research vessel, carrier and combat ship."),
            ("Inside", "Walk around a first-person bridge and approach working helm, systems and targeting panels."),
            ("Outside", "Switch to an external view while keeping the same ship position, target, damage and settings."),
            ("Performance", "Load detailed rooms near the player and budget the outside view, effects and combat simulation."),
            ("First version", "One exterior, bridge, short passage, hangar, weapon and training target form a useful playable starting point."),
        ],
        {
            "did": "Recorded the shuttle roles and research battleship concept; proposed a manageable first playable version.",
            "why": "You want a ship that feels like home, with walkable interiors and combat from its bridge.",
            "working": "The current space and planet prototype remains available. Engine tools support first-person control and selecting objects.",
            "not_working": "The capital ship, walkable rooms, targeting and combat systems have not been built or performance-tested.",
            "next": "Settle the battleship's appearance and initial bridge/hangar experience before expanding into a full ship simulation.",
        },
    )
    battleship_body = "<p>No, this is not too much for the overall game. It is a substantial new development phase.</p>"
    battleship_body += "<p>The small craft become your shuttles. The large battleship becomes the place you live, research, command and return to.</p>"
    battleship_body += "<p>My recommendation: one ship simulation shared by bridge controls and the outside camera.</p>"
    battleship_body += "<p>Lock a target at a console, switch outside, and keep that target. Damage and weapon settings stay consistent too.</p>"
    battleship_body += table(["First playable area", "What you could do"], [
        ("Battleship exterior", "Fly the large vessel in third person, with a distinct hull and visible shuttle hangar."),
        ("Bridge", "Walk in first person, see the real space scene outside, and take control at the helm."),
        ("Tactical console", "Select a training target, fire one weapon, and test basic shield and hull damage."),
        ("Hangar", "Reach it through a short passage, launch a shuttle, and return to the same battleship."),
    ])
    battleship_body += "<p>Research rooms, living quarters and deeper ship systems can grow from that foundation.</p>"
    battleship_body += "<p>For browser performance, I would load nearby interior sections and keep character movement relative to the ship.</p>"
    battleship_body += "<p>Only relevant rooms and outside detail need rendering. Ship systems still share one persistent state.</p>"
    battleship_body += "<p>Art, walking collision, docking, targeting, weapons and damage need real implementation and testing. No full-ship performance promise yet.</p>"
    battleship_body += '<p>The engine supplies <a href="https://threejs.org/docs/pages/PointerLockControls.html">first-person controls</a>, '
    battleship_body += '<a href="https://threejs.org/docs/pages/Raycaster.html">object selection</a>, and '
    battleship_body += '<a href="https://threejs.org/docs/pages/Object3D.html">local transforms and visibility controls</a>.</p>'
    battleship_body += "<p>Those are building blocks, not a finished ship simulator. This turn records the design; game code remains unchanged.</p>"
    battleship += decide.info_thread("aquila-direction-r11-info", "A walkable research battleship is a feasible direction, built in stages.",
        body_html=battleship_body, short="Research battleship and bridge")
    battleship += decide.whats_next("aquila-direction-r11-next",
        "Continue with the battleship's visual direction and first bridge/hangar experience. The existing playable preview remains available.",
        short="Shape the first battleship", follows="aquila-direction-r10-next")
    rounds = page.entry(11, "A ship to live aboard", battleship, when="2026-11-15 UTC") + rounds
    requirements = decide.recap(
        "Yes. Interior rooms can be separate spaces without fitting the hull perfectly. "
        "Your scale targets, crew plans, hangars, shields and quarters now guide the design. "
        "Windows should show the real surroundings; launch bays should connect believably. These are future requirements, not work started now.",
        [
            ("Interior", "Separate rooms are allowed; exact packing inside the exterior is unnecessary."),
            ("Scale", "Target roughly 200 times current-craft length, 75 times width and 30–50 times height."),
            ("Crew", "Plan engineering, bridge and hangars, with future crew flying combat sorties."),
            ("Combat", "The battleship carries guns and a shield with visible enemy impacts, deflections or explosions."),
            ("Home", "Personal quarters include a live space window and room for future personal features."),
        ],
        {
            "did": "Recorded the scale, flexible interior layout and future crew, combat and living-space requirements.",
            "why": "You want these needs to shape the battleship early, without building everything now.",
            "working": "The existing universe remains playable. Design records preserve these requirements for later development.",
            "not_working": "Battleship, interior rooms, crew, shields and guns remain unbuilt. Final physical dimensions remain unsettled.",
            "next": "Use these requirements when shaping the exterior and initial bridge/hangar experience; keep the broader systems deferred.",
        },
    )
    requirements_body = "<p>Yes. Separate interior rooms fit this design; they do not need to pack perfectly inside the exterior hull.</p>"
    requirements_body += "<p>Keep the connections believable: window views follow the ship, and craft leave through visible hangar openings.</p>"
    requirements_body += table(["Dimension", "Approximate target against current craft"], [
        ("Length", "200 times"),
        ("Width", "75 times"),
        ("Height", "30–50 times"),
    ])
    requirements_body += "<p>These remain relative targets until the reference shuttle and final dimensions are settled.</p>"
    requirements_body += "<p>That scale will also shape camera distance, turning speed, docking space and planetary clearance.</p>"
    requirements_body += table(["Requirement", "Design consequence"], [
        ("Original carrier silhouette", "Use Battlestar's sense of mass and carrier role as influence; give Aquila its own shape and details."),
        ("Bridge and engineering", "Reserve their roles and access early. Working panels eventually control the same ship from inside or outside."),
        ("Hangar decks", "Provide launch space for the player's shuttle and future crew-piloted combat craft."),
        ("Crew", "Allow for stations and assignments in the design. Crew implementation stays deferred."),
        ("Guns and shields", "Reserve weapon positions and shield coverage. Future impacts should visibly strike, deflect or explode against the shield."),
        ("Personal quarters", "Provide a window onto actual surrounding planets and space, with room for later personal features."),
    ])
    requirements_body += "<p>My proposed structure keeps rooms separate while sharing one ship position, target, damage and systems state.</p>"
    requirements_body += "<p>Windows get logical positions and facing directions on the vessel, even when interior dimensions differ.</p>"
    requirements_body += "<p>Nearby rooms can load as needed. Separate rooms still need explicit graphics and memory limits.</p>"
    requirements_body += "<p>These are recorded design requirements. No application code changed, and no crew, interiors or combat systems were built this turn.</p>"
    requirements += decide.info_thread("aquila-direction-r12-info", "Flexible interiors and future ship systems now guide the battleship design.",
        body_html=requirements_body, short="Battleship design requirements")
    requirements += decide.whats_next("aquila-direction-r12-next",
        "Use these requirements for the exterior and first bridge/hangar design. The full room, crew and combat systems remain deferred.",
        short="Design for later expansion", follows="aquila-direction-r11-next")
    rounds = page.entry(12, "Room to grow aboard", requirements, when="2026-11-15 UTC") + rounds
    next_build = decide.recap(
        "Start with a rough battleship exterior beside a shuttle and planet. Settle its shape, size and handling first. "
        "Then build a bridge-to-hangar launch and return loop, followed by one combat encounter. "
        "Crew, research and personal quarters follow later. This is the proposed order; no new game work has started.",
        [
            ("First", "A rough exterior establishes the battleship's silhouette, scale, hangars and flight behavior."),
            ("Compare", "Use the existing shuttle and planet to judge proportions before fixing final dimensions."),
            ("Aboard", "A bridge, short passage and hangar make the first walk, launch and return loop."),
            ("Combat", "One weapon, target and visible shield hit test shared combat from bridge and outside views."),
            ("Later", "Crew sorties, research systems, engineering and personal quarters expand the proven core."),
        ],
        {
            "did": "Recorded a proposed build order, starting with the exterior and scale comparison.",
            "why": "You asked where development should go next after setting the battleship requirements.",
            "working": "The current universe remains available; its existing scale checks remain the latest application evidence.",
            "not_working": "The battleship and interior gameplay remain unbuilt. This planning turn does not test those systems.",
            "next": "Use the rough exterior as the next implementation milestone; the broader feature set stays deferred.",
        },
    )
    next_build_body = "<p>My recommendation: build a rough battleship exterior in the current universe first.</p>"
    next_build_body += "<p>This gives us something to fly around and judge before detailed art or full interiors.</p>"
    next_build_body += table(["Order", "Playable result"], [
        ("1. Shape and scale", "Original armored hull, engine banks and large hangar openings. Compare it with a shuttle and the existing planet."),
        ("2. Life aboard", "Walk from bridge to hangar, launch your shuttle, return and resume command of the same battleship."),
        ("3. First battle", "Lock one training target, fire one weapon and see shield impacts. Bridge and outside views share combat state."),
        ("4. Expand", "Add crew sorties, engineering, research and personal quarters after the core loop works."),
    ])
    next_build_body += "<p>First milestone passes when the shuttle looks tiny, the hull reads clearly, and flight remains controllable.</p>"
    next_build_body += "<p>Check camera distance, turning, hangar access and clearance around planets using the whole hull.</p>"
    next_build_body += "<p>Start from your relative dimensions; choose the reference shuttle before fixing final metre sizes.</p>"
    next_build_body += "<p>Reserve weapon positions and window viewpoints now. Detailed rooms, textures and crew can follow.</p>"
    next_build_body += "<p>This is a proposed sequence recorded for the project. Application code remains unchanged; no new gameplay was tested.</p>"
    next_build += decide.info_thread("aquila-direction-r13-info", "Next milestone: a rough battleship exterior with a convincing sense of scale.",
        body_html=next_build_body, short="Proposed battleship build order")
    next_build += decide.whats_next("aquila-direction-r13-next",
        "The exterior and scale comparison are the recommended next build. Implementation has not started; no decision is needed now.",
        short="Shape the battleship first", follows="aquila-direction-r12-next")
    rounds = page.entry(13, "The first battleship milestone", next_build, when="2026-11-15 UTC") + rounds
    exterior = decide.recap(
        "Resolute's first exterior is built. Fly the battleship, then take Peregrine alongside and into its open hangar. "
        "Your size ratios guide this rough model; dimensions remain provisional. Planet clearance and local checks pass. "
        "Walking, crew and combat come later.",
        [
            ("Hull", "Original armored prow, raised command island, two open hangar galleries and six drives."),
            ("Scale", "Provisional length: 13.6 km. Peregrine supplies the reference for your 200× length, 75× width and 40× height ratios."),
            ("Inspection", "Fly a shuttle around the parked battleship and into the gallery; resume command without losing its position."),
            ("Clearance", "Flight boundaries and arrival routes protect the entire hull, including turns near planets."),
            ("Checks", "Production build and local browser checks pass. Physical-device performance and later interior systems remain untested."),
        ],
        {
            "did": "Built the first battleship exterior, shuttle comparison and inspection flight in the existing universe.",
            "why": "You accepted the staged plan, starting with shape, scale, handling and hangar approaches.",
            "working": "Capital flight, shuttle inspection, hangar wall contact, planet clearance, saves, return and existing controls pass local checks.",
            "not_working": "Walking, docking, weapons, shields and crew remain unbuilt. Exterior art and dimensions remain provisional.",
            "next": "Judge the exterior in the local preview. The next planned milestone is the walkable bridge and hangar loop.",
        },
    )
    exterior_body = '<p>The first research battleship exterior is ready in the <a href="http://127.0.0.1:4200/home">local preview</a>.</p>'
    exterior_body += "<p>Open Meet Resolute, choose Fly the Resolute, then use Inspect with shuttle to judge its scale up close.</p>"
    exterior_body += table(["Built", "Behavior"], [
        ("Resolute", "Original armored hull with an asymmetric command island, open lateral hangars, six drives and reserved weapon positions."),
        ("Relative size", "Peregrine measures 68 m long, 64 m wide and 16 m high. Resolute measures 13,600 × 4,800 × 640 m."),
        ("Reference", "The fleet labels a shuttle at the same physical scale. Name, dimensions and rough art remain reviewable."),
        ("Handling", "The battleship accelerates and turns more slowly. Cameras frame each craft at its own size."),
        ("Inspection", "The battleship holds position while you fly Peregrine. Open galleries are accessible; solid walls stop the shuttle."),
        ("Return and saves", "Resume command restores the parked vessel. Reloading an inspection also returns to that battleship."),
        ("Planet limits", "A conservative hull envelope protects against boosted impacts and turns. Arrival routes include the same margin."),
    ])
    exterior_body += "<p>Production application and desktop-code compilation pass. Eleven model tests and six isolated safety tests pass.</p>"
    exterior_body += "<p>Eleven existing game groups, four quality groups and eight battleship groups pass in isolated Chrome on this RTX 5090 desktop.</p>"
    exterior_body += "<p>No page or console errors occurred in those final browser runs. Narrow layouts were checked on the same desktop.</p>"
    exterior_body += "<p>Inspection is an exterior flight tool. It does not simulate walking, docking, crew, guns, shields or damage.</p>"
    exterior_body += "<p>Hull contact uses coarse sampled sweeps. Physical phones, laptops, long sessions and packaged desktop play remain untested.</p>"
    exterior_body += "<p>No downloaded art, paid generation, publication or transfers were used.</p>"
    exterior_body += page.figure(page.copy_media(HERE / "media/r14-fleet.png", out), "14.1 Resolute exterior",
                                 " Original rough hull with a labeled reference shuttle. Final name, size and art remain open.", look=True)
    exterior_body += page.figure(page.copy_media(HERE / "media/r14-hangar-approach.png", out), "14.2 Shuttle approach",
                                 " Peregrine approaching the open starboard gallery. The battleship stays parked.")
    exterior_body += page.figure(page.copy_media(HERE / "media/r14-planet.png", out), "14.3 Planet comparison",
                                 " Battleship approaching Vesper. These remain compressed game proportions.")
    exterior_body += page.figure(page.copy_media(HERE / "media/r14-phone-fleet.png", out), "14.4 Narrow layout",
                                 " Fleet controls fit a narrow viewport. This is not a physical-phone performance result.")
    exterior += decide.info_thread("aquila-direction-r14-info", "Resolute's rough exterior, shuttle inspection and full-hull planet clearance are built locally.",
        body_html=exterior_body, short="First battleship exterior")
    exterior += decide.whats_next("aquila-direction-r14-next",
        "Exterior ready for visual review. The next planned milestone is the walkable bridge, passage and hangar loop.",
        short="Review shape and scale", follows="aquila-direction-r13-next")
    rounds = page.entry(14, "Resolute takes shape", exterior, when="2026-11-15 UTC") + rounds
    orbit_update = decide.recap(
        "Right-drag now turns your view around the ship. Release it to return behind. Left-drag still steers. "
        "Resolute is twice as high; length and width stay the same. Local checks pass.",
        [
            ("Steering", "Left-drag steers your ship; keyboard and touch controls still work."),
            ("Camera", "Hold right and drag to look around, above or below. Release to return behind your ship."),
            ("Height", "Resolute now stands 1,280 metres high, keeping its 13,600-metre length and 4,800-metre width."),
            ("Hangars", "Hangar openings and approach points follow the taller hull."),
            ("Checks", "Local build, camera, ship and existing game checks pass."),
        ],
        {
            "did": "Added right-button camera orbit and doubled the battleship's height.",
            "why": "You wanted to look around without steering and give the hull more height.",
            "working": "Release returns behind the ship. Orbit also works during shuttle inspection and planetary flight.",
            "not_working": "Physical-device checks and finished exterior art remain outstanding. Interior gameplay remains a later milestone.",
            "next": "Changes are ready in the local preview; no decision is required now.",
        },
    )
    orbit_body = '<p>Try the updated controls in the <a href="http://127.0.0.1:4200/play">local game preview</a>.</p>'
    orbit_body += table(["Input", "Result"], [
        ("Left mouse", "Hold and drag to steer."),
        ("Right mouse", "Hold and drag to orbit around your current ship, including the shuttle."),
        ("Release right", "Smooth return behind the ship. Reduced motion returns directly."),
    ])
    orbit_body += "<p>Camera movement preserves heading, surface altitude and assisted travel. Return follows an arc around the hull.</p>"
    orbit_body += "<p>Height changed from 640 to 1,280 metres. Hull clearance and hangar positions follow the new height.</p>"
    orbit_body += "<p>Production build and eleven model tests pass. Six camera groups, eight battleship groups and eleven existing game groups pass.</p>"
    orbit_body += "<p>Final Chrome runs reported no page or console errors. Both button orders and interrupted input were checked.</p>"
    orbit_body += "<p>These are local desktop results. Physical phones, other browser engines and long sessions remain untested.</p>"
    orbit_body += page.figure(page.copy_media(HERE / "media/r15-orbit-side.png", out), "15.1 Right mouse held",
                              " Side view during camera orbit. The battleship stays still.", look=True)
    orbit_body += page.figure(page.copy_media(HERE / "media/r15-battleship-fleet.png", out), "15.2 Taller Resolute",
                              " Height doubled; length and width retained. Exterior remains a rough model.")
    orbit_update += decide.info_thread("aquila-direction-r15-info", "Right-drag now orbits the camera, and Resolute is twice as high.",
        body_html=orbit_body, short="Camera orbit and taller hull")
    orbit_update += decide.whats_next("aquila-direction-r15-next",
        "Changes are ready in the local preview. The next planned milestone remains the walkable bridge and hangar loop.",
        short="Ready to fly", follows="aquila-direction-r14-next")
    rounds = page.entry(15, "Look around Resolute", orbit_update, when="2026-11-15 UTC") + rounds
    aboard_plan = decide.recap(
        "Next: go inside Resolute. Build a small bridge, working helm and short passage to the hangar. "
        "Walk aboard, launch your shuttle, return and take command again. That gives you one complete loop aboard your ship.",
        [
            ("Bridge", "Walk in first person, with windows showing the space around your battleship."),
            ("Helm", "Take command from the bridge and switch to the outside flight view."),
            ("Passage", "Connect the bridge to one hangar with a short walk."),
            ("Shuttle", "Board Peregrine, launch, return and resume command aboard Resolute."),
            ("Scope", "Start with this complete loop. Crew, combat and other rooms follow later."),
        ],
        {
            "did": "Recorded your acceptance of the current controls and proportions, and outlined the next interior milestone.",
            "why": "You asked what comes next after confirming the latest changes work.",
            "working": "Exterior flight and camera controls are locally tested and now confirmed by you.",
            "not_working": "Walkable interiors and their launch and return loop are not built yet.",
            "next": "Build the bridge, helm, passage and hangar loop as the next milestone in the existing plan.",
        },
    )
    aboard_body = "<p>My recommendation: make Resolute a place you can walk around and command from inside.</p>"
    aboard_body += table(["Step", "Playable result"], [
        ("1. Bridge", "Enter in first person. Windows show the planets and stars around your actual ship."),
        ("2. Helm", "Use the helm to fly; switch between bridge and outside views, then leave the controls."),
        ("3. Hangar", "Walk through a short passage, board Peregrine and launch into space."),
        ("4. Return", "Return to the hangar, step out and take command of the same battleship again."),
    ])
    aboard_body += "<p>Use a simple guided return first. Rooms can remain separate from the exterior, as you requested.</p>"
    aboard_body += "<p>The ship keeps its position and state throughout. Crew, combat, engineering and quarters follow this complete loop.</p>"
    aboard_body += "<p>You confirmed the current controls and taller hull work and like the result.</p>"
    aboard_body += "<p>This turn records the plan. Application code is unchanged; interior gameplay has not been built or tested.</p>"
    aboard_plan += decide.info_thread("aquila-direction-r16-info", "Next: a walkable bridge and hangar with a complete shuttle launch and return loop.",
        body_html=aboard_body, short="Life aboard Resolute")
    aboard_plan += decide.whats_next("aquila-direction-r16-next",
        "The next milestone is the bridge, helm, passage and hangar loop. This turn defines that scope; implementation has not started.",
        short="Build the first interior loop", follows="aquila-direction-r15-next")
    rounds = page.entry(16, "Go inside Resolute", aboard_plan, when="2026-11-15 UTC") + rounds
    interior_update = decide.recap(
        "You can walk inside Resolute, fly from the bridge and take Peregrine out from the hangar. "
        "Return brings you aboard the same battleship. Windows show nearby space. The first loop works locally; rooms remain rough.",
        [
            ("Bridge", "Walk around a human-scale command deck with windows onto the surrounding universe."),
            ("Helm", "Take the helm to fly Resolute; switch outside and return without changing vessels."),
            ("Hangar", "Walk through the passage, approach Peregrine's station and launch."),
            ("Return", "Use the guided return to disembark, walk back and resume command."),
            ("Checks", "Local build, walking, flight, saves and browser input checks pass. Physical-device performance remains untested."),
        ],
        {
            "did": "Built the bridge, helm, passage, hangar and complete shuttle launch and return loop.",
            "why": "You approved making Resolute a ship you can walk around and command from inside.",
            "working": "Walking, live windows, bridge flight, exterior switching, shuttle return, pause and saves pass local checks.",
            "not_working": "Crew, combat and other rooms remain unbuilt. Return is guided and immediate; interiors remain prototype art.",
            "next": "Try the interior in the local preview. No decision is required now.",
        },
    )
    interior_body = '<p>Choose Enter bridge while flying Resolute in the <a href="http://127.0.0.1:4200/play">local preview</a>.</p>'
    interior_body += table(["Place", "Controls"], [
        ("Walking", "WASD walks; Shift runs. Click to look, or use dragging and arrow keys. Escape releases the cursor and pauses."),
        ("Helm", "Approach the console and press F, or use Take helm. Existing flight controls now fly from the bridge."),
        ("Outside view", "Press C or choose Exterior view. Return aboard restores your room or helm mode during this visit."),
        ("Hangar", "Leave the helm with F, then follow the aft passage. Approach the boarding station and launch Peregrine."),
        ("Return", "Choose Return to hangar. Resolute keeps its parked position; walk back to take command again."),
    ])
    interior_body += "<p>Windows show the actual nearby world. Interior rooms use separate geometry, as requested.</p>"
    interior_body += "<p>One graphics canvas handles both views. Repeated visits reused the room resources during local checks.</p>"
    interior_body += "<p>Production build and fourteen model tests pass. Seven interior groups and twenty-five existing browser groups pass.</p>"
    interior_body += "<p>Final Chrome runs reported no page or console errors. Mouse capture, interrupted input, quality choices and narrow controls were checked.</p>"
    interior_body += "<p>Return is immediate, without a docking approach. Reload resumes exterior battleship flight; your walking position lasts only this visit.</p>"
    interior_body += "<p>Physical phones, other browser engines and long play remain untested. Crew, combat, engineering and quarters remain later work.</p>"
    interior_body += page.figure(page.copy_media(HERE / "media/r17-interior-bridge.png", out), "17.1 Walkable bridge",
                                 " Nearby space shows through the forward windows. Rooms remain prototype art.", look=True)
    interior_body += page.figure(page.copy_media(HERE / "media/r17-interior-helm.png", out), "17.2 Command from inside",
                                 " The helm flies the same battleship; the view outside changes with its heading and position.")
    interior_body += page.figure(page.copy_media(HERE / "media/r17-interior-returned.png", out), "17.3 Back aboard",
                                 " Returned to Peregrine's boarding station after a shuttle flight.")
    interior_body += page.figure(page.copy_media(HERE / "media/r17-interior-mobile.png", out), "17.4 Narrow controls",
                                 " Walking and station controls fit a narrow viewport. This is a desktop layout check.")
    interior_update += decide.info_thread("aquila-direction-r17-info", "Resolute now has a walkable bridge, working helm and connected shuttle hangar.",
        body_html=interior_body, short="First complete interior loop")
    interior_update += decide.whats_next("aquila-direction-r17-next",
        "The first interior loop is ready for your review. Further room polish and the planned combat milestone wait for direction.",
        short="Walk aboard Resolute", follows="aquila-direction-r16-next")
    rounds = page.entry(17, "Life aboard Resolute", interior_update, when="2026-11-15 UTC") + rounds
    camera_fix = decide.recap(
        "Added protection against sudden walking-camera jumps. Oversized mouse movements are discarded, and cursor capture starts cleanly. "
        "Local checks pass. I reproduced the jump using an injected spike; your exact mouse event remains unconfirmed.",
        [
            ("Spikes", "Suspicious mouse movements no longer snap the walking view up or down in the regression test."),
            ("Capture", "Capture changes clear old drag positions; direct mouse input is requested where supported."),
            ("Feel", "Normal mouse movements retain their sensitivity and continuous turning range."),
            ("Checks", "Focused input checks, the full interior loop, exterior camera checks and production build pass."),
            ("Retry", "Refresh the game and try walking again to check your intermittent case."),
        ],
        {
            "did": "Guarded walking camera input against sudden mouse spikes and cleaned up cursor capture transitions.",
            "why": "You reported occasional upward or downward snaps while walking inside Resolute.",
            "working": "Injected spikes are rejected; normal looking, walking, dragging, capture, pause and the interior loop pass local checks.",
            "not_working": "Your exact hardware event was not captured, so the intermittent case still needs a retry.",
            "next": "Refresh, re-enter the bridge and report whether the camera still snaps.",
        },
    )
    camera_fix_body = "<p>A deliberately injected mouse movement caused a 74.48° vertical jump before the fix.</p>"
    camera_fix_body += "<p>The same input now leaves pitch unchanged. Valid movement on the other axis still works.</p>"
    camera_fix_body += "<p>Rejected movement is discarded, never replayed later. Normal continuous turns retain their range.</p>"
    camera_fix_body += "<p>Capture changes reset old drag positions. Ordinary capture and dragging remain available when direct mouse input is unsupported.</p>"
    camera_fix_body += "<p>Production build and sixteen model tests pass. Five focused checks, seven interior groups and six exterior camera groups pass.</p>"
    camera_fix_body += "<p>Final Chrome runs reported no page or console errors. These checks do not establish the exact cause on your setup.</p>"
    camera_fix += decide.info_thread("aquila-direction-r18-info", "Walking camera input now rejects sudden mouse spikes.",
        body_html=camera_fix_body, short="Walking camera protection")
    camera_fix += decide.request("aquila-direction-r18-check",
        "Refresh the game, re-enter the bridge and try walking again.",
        info='<p>This checks whether the fix covers the intermittent jump on your setup. Use the <a href="http://127.0.0.1:4200/play">local game</a>.</p>',
        short="Check the walking camera",
        reply="If it still snaps, were you using captured mouse, dragging or keyboard alone?")
    camera_fix += decide.whats_next("aquila-direction-r18-next",
        "Waiting for the walking retry. If it still jumps, the control mode will help narrow the remaining cause.",
        short="Check the intermittent jump", follows="aquila-direction-r17-next")
    rounds = page.entry(18, "Steadier walking camera", camera_fix, when="2026-11-15 UTC") + rounds
    camera_confirmed = decide.recap(
        "You report that the walking camera seems fixed. Confirmation saved; the retry request is closed. No further game changes this turn.",
        [
            ("Result", "Your retry suggests the camera snap is resolved."),
            ("Record", "Saved your confirmation in the project checkpoint."),
            ("Request", "Closed the walking retry request."),
            ("Code", "No further game changes or test runs this turn."),
            ("Next", "Nothing needed from you now."),
        ],
        {
            "did": "Recorded your successful retry and closed the camera check.",
            "why": "You said the camera seems fixed now.",
            "working": "Walking looks stable in your retry; the earlier local checks passed.",
            "not_working": "No further camera issue reported. The original hardware event was not captured.",
            "next": "No camera work pending; further features wait for direction.",
        },
    )
    camera_confirmed += decide.info_thread("aquila-direction-r19-info",
        "You report that the walking camera seems fixed.",
        body_html="<p>Saved your chat confirmation and closed the retry request. Game code and previous test evidence are unchanged.</p>",
        short="Camera retry confirmed", follows="aquila-direction-r18-check")
    camera_confirmed += decide.whats_next("aquila-direction-r19-next",
        "No camera work pending. Further interior or combat work waits for direction.",
        short="Camera check complete", follows="aquila-direction-r18-next")
    rounds = page.entry(19, "Camera retry confirmed", camera_confirmed, when="2026-11-15 UTC") + rounds
    combat_plan = decide.recap(
        "The first ship loop works: fly, walk aboard, command from the bridge, launch a shuttle and return. "
        "Next: one practice fight. Lock a target, fire a weapon and see shields take hits. "
        "Bridge and outside views share the same battle. Crew, research and quarters come later.",
        [
            ("Foundation", "The glass website, space flight, planet visits and first interior loop work locally."),
            ("Target", "Add a bridge tactical console to select and lock one training drone."),
            ("Weapon", "Give Resolute one working gun with visible fire and hits."),
            ("Defense", "Let the drone shoot back, showing shield impacts and hull damage when shields fail."),
            ("Views", "Keep the same target and damage when switching between bridge and outside views."),
        ],
        {
            "did": "Reviewed the working foundation and outlined the next planned combat milestone.",
            "why": "You asked where development stands and what comes next.",
            "working": "Local flight, planet visits, bridge walking, helm control and shuttle launch and return work. Your camera retry passed.",
            "not_working": "Combat is not built yet. Crew, research, engineering and quarters remain later work.",
            "next": "Build one repeatable practice encounter as the next proposed milestone.",
        },
    )
    combat_body = "<p>The exploration and interior foundation is playable. Next recommendation: give Resolute one complete practice fight.</p>"
    combat_body += table(["Working now", "Current state"], [
        ("Website and space", "Glass controls over the shared world, free flight, large planets and shuttle visits."),
        ("Resolute", "Battleship exterior, camera orbit, walkable bridge, working helm and connected hangar."),
        ("Ship loop", "Launch Peregrine, fly out, return aboard and resume command. Your walking camera retry now appears stable."),
    ])
    combat_body += table(["Next step", "Playable result"], [
        ("Tactical console", "Walk to a bridge station and lock one training drone."),
        ("One weapon", "Fire a battleship gun with visible shots and confirmed hits."),
        ("Shields and hull", "The drone shoots back. Impacts flash on the shield; depleted shields expose the hull."),
        ("Shared battle", "Switch between bridge and exterior control without resetting target, shields or damage."),
        ("Practice reset", "Restart the encounter to test the whole loop again."),
    ])
    combat_body += "<p>This gives the bridge a tactical role and turns the ship systems into a repeatable gameplay loop.</p>"
    combat_body += "<p>Crew sorties, research, engineering, quarters and richer art follow later. Current rooms and hull remain prototype art.</p>"
    combat_body += "<p>This turn records the recommendation. Combat implementation has not started; game code and previous test evidence are unchanged.</p>"
    combat_plan += decide.info_thread("aquila-direction-r20-info",
        "The first ship loop works; the next proposed milestone is one practice battle.",
        body_html=combat_body, short="First combat encounter")
    combat_plan += decide.whats_next("aquila-direction-r20-next",
        "The proposed next build is a tactical console, training drone, weapon and visible shield hits. Implementation waits for direction.",
        short="Make Resolute fight", follows="aquila-direction-r19-next")
    rounds = page.entry(20, "First practice battle", combat_plan, when="2026-11-15 UTC") + rounds
    combat_update = decide.recap(
        "Resolute can fight a training drone from the bridge or outside. Lock Sentinel, fire the pulse lance, "
        "and watch shields take hits before hull damage. Both views share the battle. Reset restores both ships. "
        "Local checks pass; the drone and rooms remain prototype art.",
        [
            ("Station", "Use the tactical console beside the helm, or start practice from the exterior view."),
            ("Weapon", "Press T to lock Sentinel; hold X to fire the pulse lance."),
            ("Defense", "Sentinel fires back once engaged. Shields absorb hits before hull damage."),
            ("Reset", "Victory or defeat stops both guns. Reset practice restores both ships without moving Resolute."),
            ("Checks", "Production build, twenty-two model tests, six safety tests and thirty-seven browser groups pass."),
        ],
        {
            "did": "Built the tactical station, training drone, pulse lance, shield impacts, hull damage and repeatable battle.",
            "why": "You approved the first practice encounter so the bridge has a working combat role.",
            "working": "Bridge and exterior share target, damage and outcome. Pause, reset, victory, defeat and existing flight checks pass.",
            "not_working": "The drone stays in place. General enemy tactics, crew combat, persistent damage and final art remain future work.",
            "next": "The local practice battle is ready to try. No decision is required now.",
        },
    )
    combat_update_body = '<p>Open the <a href="http://127.0.0.1:4200/play">local game</a> with Resolute selected.</p>'
    combat_update_body += table(["Action", "Control"], [
        ("Start outside", "Choose Start practice during free battleship flight."),
        ("Start aboard", "Walk to the tactical console beside the helm. Press F, then choose Start practice."),
        ("Fight", "Press T to lock Sentinel. Hold X or click Fire pulse. Sentinel returns fire after engagement."),
        ("Change views", "Press C to switch outside. Return aboard keeps the same target, shields, hull and outcome."),
        ("Repeat", "Reset practice restores both ships. End practice returns to exploration."),
    ])
    combat_update_body += "<p>One foredeck gun tracks Sentinel. Visible beams, expanding shield impacts and hull flashes show each hit.</p>"
    combat_update_body += "<p>Weapons respect range and planets blocking the firing line. The pulse lance also requires the target ahead.</p>"
    combat_update_body += "<p>Pause, settings and lost focus freeze combat. Walking away from a station prevents player fire.</p>"
    combat_update_body += "<p>Training damage is temporary. Reload, travel, shuttle launch and craft changes end the exercise.</p>"
    combat_update_body += "<p>Production build, twenty-two model tests, six safety tests and thirty-seven browser groups pass. Browser runs reported no application errors.</p>"
    combat_update_body += "<p>Checks cover victory, defeat, shield failure, view switching, controls, resource reuse, narrow layouts and existing exploration.</p>"
    combat_update_body += "<p>Sentinel stays at its deployed position; pulses hit instantly. General enemy tactics and persistent ship damage remain unbuilt.</p>"
    combat_update_body += "<p>Physical phones, other browser engines and long play remain untested. Rooms and ships still use prototype art.</p>"
    combat_update_body += page.figure(page.copy_media(HERE / "media/r21-combat-station.png", out), "21.1 Tactical console",
                                      " Walk to the station beside the helm. F takes tactical control.", look=True)
    combat_update_body += page.figure(page.copy_media(HERE / "media/r21-combat-bridge-fire.png", out), "21.2 Fire from the bridge",
                                      " The pulse lance fires into the same world seen through the windows.")
    combat_update_body += page.figure(page.copy_media(HERE / "media/r21-combat-exterior-fire.png", out), "21.3 Shared exterior battle",
                                      " The target and both ships' damage persist when changing views.")
    combat_update_body += page.figure(page.copy_media(HERE / "media/r21-combat-mobile.png", out), "21.4 Narrow controls",
                                      " Combat buttons fit the narrow desktop viewport. This does not establish phone performance.")
    combat_update += decide.info_thread("aquila-direction-r21-info",
        "Resolute now has a complete practice battle, controlled from the bridge or outside.",
        body_html=combat_update_body, short="First playable battle")
    combat_update += decide.whats_next("aquila-direction-r21-next",
        "The practice battle is ready in the local preview. Further combat features wait for direction; no decision is required now.",
        short="Practice battle ready", follows="aquila-direction-r20-next")
    rounds = page.entry(21, "Resolute takes fire", combat_update, when="2026-11-15 UTC") + rounds
    automatic = decide.recap(
        "Weapons now find targets, aim and fire themselves. Set Automatic or Hold fire at the console. "
        "They keep operating while you fly or walk aboard. Manual lock and trigger controls are removed.",
        [
            ("Automatic", "Starting practice lets the ship acquire Sentinel and fire on its own."),
            ("Console", "Choose Automatic or Hold fire; the same mode carries between bridge and exterior views."),
            ("Walking", "Weapons keep operating when you leave the console and walk aboard."),
            ("Tracking", "Leaving range stops fire. Returning automatically reacquires the target."),
            ("Checks", "Production build, twenty-three model tests and eight focused browser groups pass."),
        ],
        {
            "did": "Changed practice weapons to automatic acquisition, aiming and firing, with two console modes.",
            "why": "You clarified that ship systems handle weapons while the player changes modes at consoles.",
            "working": "Automatic fire, Hold fire, walking, view changes, target reacquisition, reset, pause and recovery pass local checks.",
            "not_working": "The exercise still has one stationary target. General enemy tactics and detailed turret restrictions remain future work.",
            "next": "The local preview now follows your automated-weapons design. No decision is required.",
        },
    )
    automatic_body = '<p>Choose Start practice in the <a href="http://127.0.0.1:4200/play">local game</a>. The ship handles targeting and shots.</p>'
    automatic_body += table(["Mode", "Behavior"], [
        ("Automatic", "Acquire, aim and fire at a valid target. Continue while you fly or walk aboard."),
        ("Hold fire", "Stop outgoing shots while sensors remain active. Enemy fire continues once the encounter has begun."),
    ])
    automatic_body += "<p>Change modes at tactical or the helm, or from the exterior controls. Both views share the same choice.</p>"
    automatic_body += "<p>Manual lock and trigger controls are removed. You do not need to point the ship's bow at Sentinel.</p>"
    automatic_body += "<p>Range and planets still limit fire. Tracking recovers automatically when the target becomes reachable again.</p>"
    automatic_body += "<p>Reset restores both ships and selects Automatic. Pause freezes combat; walking away from the console does not.</p>"
    automatic_body += "<p>Production build, twenty-three model tests and eight browser groups pass. No browser application errors were reported.</p>"
    automatic_body += "<p>This remains one training drone with temporary damage. Detailed turret restrictions, general enemy tactics and physical-device testing remain future work.</p>"
    automatic_body += page.figure(page.copy_media(HERE / "media/r22-auto-hold-fire.png", out), "22.1 Console mode",
                                  " Hold fire stops outgoing shots. Incoming fire continues after engagement.", look=True)
    automatic_body += page.figure(page.copy_media(HERE / "media/r22-auto-mobile.png", out), "22.2 Automatic weapons",
                                  " The ship acquires and fires on its own. Narrow controls show the current mode.")
    automatic += decide.info_thread("aquila-direction-r22-info",
        "Weapons now acquire, aim and fire automatically; the console changes their operating mode.",
        body_html=automatic_body, short="Automatic ship weapons")
    automatic += decide.whats_next("aquila-direction-r22-next",
        "The corrected practice battle is ready locally. Further combat features wait for direction; no decision is required now.",
        short="Automatic combat ready", follows="aquila-direction-r21-next")
    rounds = page.entry(22, "Weapons handle themselves", automatic, when="2026-11-15 UTC") + rounds
    combat_rules = decide.recap(
        "Automatic weapons are settled. My proposed rules put your decisions into ship position, equipment, power and orders. "
        "Every weapon needs a purpose, cost and counter. Start with beams, cannons, missiles and point defense. "
        "First discussion: how fast large battles should feel.",
        [
            ("Command", "You set modes and priorities; weapons handle targeting, aiming and shots."),
            ("Roles", "Give each weapon a useful target and a clear weakness."),
            ("Limits", "Range, tracking, firing arcs, heat, power and ammunition should shape weapon choices."),
            ("Damage", "Proposed layers: shields, armor, hull and disabled ship systems."),
            ("Pace", "My recommendation: deliberate battleship decisions with fast fighters and missiles around them."),
        ],
        {
            "did": "Recorded your automation confirmation and drafted broader weapon and combat rules.",
            "why": "You want a consistent foundation for a game with substantial combat and many weapons.",
            "working": "The existing automatic practice battle remains unchanged. Automation is confirmed as the intended design.",
            "not_working": "Expanded weapon families, damage layers and battle rules are proposals, not implemented systems.",
            "next": "Discuss battle pace first, then settle defenses, weapon costs and player orders.",
        },
    )
    rules_body = "<p>You confirmed automatic weapons. The rules below are my starting proposal for discussion.</p>"
    rules_body += table(["Proposed rule", "Effect on play"], [
        ("Command the ship", "Choose positioning, equipment, power and operating modes. Useful defaults keep the ship fighting without constant adjustments."),
        ("Give every weapon a role", "Each needs a purpose, cost and counter. Larger guns sacrifice tracking, coverage or resources."),
        ("Respect the battlefield", "Turrets track within firing arcs. Range, obstacles and target movement affect shots without requiring manual aim."),
        ("Distribute fire intelligently", "Defense guns prioritize incoming threats. Heavy guns choose suitable large targets and avoid wasting whole volleys."),
        ("Make damage change behavior", "Shields take impacts, armor limits hull damage, and damaged engines, sensors or guns lose capability."),
        ("Make threats readable", "Show dangerous volleys and shield impacts clearly. Retreat, interception and countermeasures should give meaningful responses."),
        ("Give crew craft broad orders", "Future escort, intercept and strike orders guide sorties. Pilots handle their own weapons."),
        ("Keep shared rules", "Enemies obey equipment limits. Camera or graphics quality changes must not change combat outcomes."),
    ])
    rules_body += table(["Proposed family", "Main job", "Tradeoff"], [
        ("Beams", "Reliable tracking and sustained shield pressure", "Power draw, heat and limited burst damage"),
        ("Cannons", "Heavy hull and armor pressure", "Travel time, ammunition and slower heavy mounts"),
        ("Missiles and torpedoes", "Guided attacks and heavy volleys", "Ammunition costs; defenders can intercept them"),
        ("Point defense", "Stop incoming munitions and screen small craft", "Short range and weak capital-ship damage"),
    ])
    rules_body += "<p>These are proposed game roles. Exact damage modifiers, ranges and firing rates remain undecided.</p>"
    rules_body += "<p>Shield direction, repairs, ship loss, ammunition logistics and electronic warfare need later discussion.</p>"
    rules_body += "<p>Only design records changed. Existing game code and test evidence are unchanged; no new runtime checks were needed.</p>"
    combat_rules += decide.info_thread("aquila-direction-r23-info",
        "Automation is confirmed; broader weapon and combat rules are now drafted for discussion.",
        body_html=rules_body, short="Proposed combat foundation")
    pace_id = "aquila-direction-r23-pace"
    pace_choices = table(["Battle pace", "Experience", "Choose"], [
        ("Deliberate battles, fast fighters", "Time for battleship decisions, with fast craft and missiles keeping the scene active.",
         decide.pick(pace_id, "mixed", "Deliberate battles, fast fighters", mode="single", mine=True)),
        ("Fast, constant action", "Frequent movement and quick outcomes; less time to inspect ship systems.",
         decide.pick(pace_id, "fast", "Fast, constant action", mode="single")),
        ("Slow, tactical engagements", "Longer exchanges with more time for positioning, system choices and planning.",
         decide.pick(pace_id, "slow", "Slow, tactical engagements", mode="single")),
    ])
    combat_rules += decide.question(pace_id, "What should large battles feel like?", [],
        short="Choose the battle pace", anchors_html=pace_choices,
        why="I recommend deliberate battles with fast fighters. This leaves room for ship decisions while keeping combat active.")
    combat_rules += decide.whats_next("aquila-direction-r23-next",
        "Continue the design discussion after your pace preference. The proposed rules do not authorize another implementation milestone.",
        short="Discuss the combat rules", follows="aquila-direction-r22-next")
    rounds = page.entry(23, "Rules for ship combat", combat_rules, when="2026-11-15 UTC") + rounds
    pace_clarification = decide.recap(
        "I mean big ships trade several volleys, giving you time to change tactics. "
        "Your guns keep working automatically. You can turn, reinforce shields, launch interceptors or retreat. "
        "Fighters and missiles still move fast. Exact battle length remains open.",
        [
            ("Time", "A large-ship fight leaves room for several decisions before its outcome is settled."),
            ("Weapons", "Guns keep aiming and firing automatically throughout."),
            ("Action", "Fighters and missiles can stay fast, with frequent visible weapons fire."),
            ("Example", "Spot incoming torpedoes, reinforce shields and order an interception while the guns keep shooting."),
            ("Scope", "This explains the proposed feel. Battle length and new ship systems remain undecided."),
        ],
        {
            "did": "Explained the proposed battle pace in concrete ship decisions.",
            "why": "You asked what deliberate battles means.",
            "working": "Automatic weapon behavior remains confirmed; existing game code is unchanged.",
            "not_working": "No battle pace selected yet. Shield power changes and fighter orders are examples of future systems.",
            "next": "Check whether this matches the combat feel you want.",
        },
    )
    pace_clarification += decide.info_thread("aquila-direction-r24-info",
        "By deliberate, I mean big ships trade several volleys, giving you time to change tactics.",
        body_html="<p>Your weapons keep shooting automatically. Fighters and missiles can still move fast.</p>"
                  "<p>You would have time to turn the ship, reinforce shields, launch interceptors or retreat.</p>"
                  "<p>For example, you spot incoming torpedoes and order an interception while your guns keep firing.</p>"
                  "<p>Shield power and fighter orders illustrate the proposed future systems. Exact battle duration remains open; no gameplay changed.</p>",
        short="Time for ship decisions")
    clarified_id = "aquila-direction-r24-pace"
    clarified_choices = table(["Preference", "Choose"], [
        ("Yes, time for ship decisions", decide.pick(clarified_id, "mixed", "Yes, time for ship decisions", mode="single", mine=True)),
        ("Faster and more frantic", decide.pick(clarified_id, "fast", "Faster and more frantic", mode="single")),
        ("Slower with deeper planning", decide.pick(clarified_id, "slow", "Slower with deeper planning", mode="single")),
    ])
    pace_clarification += decide.question(clarified_id,
        "Does that match the kind of battles you have in mind?", [],
        short="Clarify the battle feel", anchors_html=clarified_choices,
        why="My recommendation leaves time for ship decisions while weapons, fighters and missiles keep the battle active.",
        follows="aquila-direction-r23-pace")
    pace_clarification += decide.whats_next("aquila-direction-r24-next",
        "Continue discussing the battle feel. Weapon rules remain a draft; this does not authorize new gameplay implementation.",
        short="Continue the combat discussion", follows="aquila-direction-r23-next")
    rounds = page.entry(24, "Time to change tactics", pace_clarification, when="2026-11-15 UTC") + rounds
    projectiles = decide.recap(
        "Deliberate battles are settled. Start capable, then grow through better weapons, defenses and evasion. "
        "Ordinary weapons need visible travel and real chances to miss. "
        "Two basic kinetic turrets and one shield form the starting direction. "
        "The current instant pulse still needs replacement; this round records design only.",
        [
            ("Pace", "Tactical decisions have time to matter; impacts remain fast, explosive and satisfying."),
            ("Travel", "Ordinary weapons are projectiles. Instant long-range attacks belong only in future rare-equipment notes."),
            ("Growth", "Individual turret upgrades, stronger defenses, countermeasures and automatic evasion support different ship builds."),
            ("Starter", "Two basic kinetic turrets and one simple shield establish the initial direction."),
            ("Scope", "The thirty-turret example is provisional. Gameplay remains unchanged while we discuss these rules."),
        ],
        {
            "did": "Recorded your projectile combat and progression direction, closing the battle-pace question.",
            "why": "You want tactical battles and a capable starting ship with substantial room to grow.",
            "working": "Existing automatic combat remains available. The new rules distinguish your decisions from my design proposals.",
            "not_working": "The prototype still uses instant pulse hits. Projectile travel, expanded defenses and automatic evasion remain unbuilt.",
            "next": "Define the starter encounter before expanding the weapon catalog and upgrade tree.",
        },
    )
    projectile_body = "<p>You selected deliberate battles directly in chat. The earlier ordinary-beam proposal is superseded.</p>"
    projectile_body += table(["Your direction", "Design consequence"], [
        ("Capable starter, substantial growth", "Begin with useful combat systems at their lowest level. Upgrade offense and defense over time."),
        ("Traveling weapons", "No ordinary hitscan or effectively instant long-range attacks. Keep rare superweapons only in future notes, with major cooldowns."),
        ("Simple starting shield", "Absorb a few hits, then fail. Later choices include stronger shields, sustain, multiple shields and special armor."),
        ("Tank or avoid damage", "Support shieldless armor builds, countermeasures and upgradeable automatic evasion. Exact starter evasion remains open."),
        ("Guidance can fail", "Disruption breaks tracking. Straight flight or the last known position remain possible lost-lock behaviors."),
        ("Individual turret growth", "Start with two basic kinetic turrets, left and right. Around thirty later mounts illustrates growth, not a fixed cap."),
    ])
    projectile_body += "<p>Your year-of-play example describes long-term growth. It does not set a required unlock schedule.</p>"
    projectile_body += table(["My proposed rule", "Why it matters"], [
        ("Resolve damage on arrival", "Automatic aim predicts movement, but a projectile can miss. A moving tracer cannot conceal instant damage."),
        ("Limit guidance and evasion", "Missiles have finite turning ability. Evasion uses actual ship movement; it never grants guaranteed immunity."),
        ("Give mounts real limits", "Tracking, firing arcs and hull obstruction reward positioning. Power, cooling and ammunition constrain powerful loadouts."),
        ("Keep upgrade choices useful", "Separate ship capacity, individual turret development and installed equipment. Stronger equipment still has weaknesses."),
        ("Preserve readable battles", "Late-game defenses and salvo pacing must grow alongside firepower. More turrets must not erase reaction time."),
    ])
    projectile_body += table(["Proposed projectile family", "Job", "Weakness"], [
        ("Autocannons", "Streams or bursts of basic kinetic rounds", "Travel time and limited heavy-armor pressure"),
        ("Heavy kinetic cannons", "Large solid rounds with forceful impacts", "Slow reload and tracking"),
        ("Flak and point defense", "Projectile guns or interceptor missiles defend against incoming threats", "Limited range and saturation"),
        ("Unguided rockets", "Explosive salvos aimed ahead of moving targets", "No course correction"),
        ("Guided missiles", "Tracking threats with visible travel", "Decoys, jamming, interception and evasion"),
        ("Heavy torpedoes", "Powerful attacks against capital ships", "Slow approach and limited ammunition"),
        ("Disruption rounds", "Delivered payload pressures shields or ship systems", "Specialized effect and limited hull damage"),
        ("Nuclear or siege warheads", "Later large-area attacks and major spectacle", "Scarcity, long reload, interception and dangerous blast proximity"),
    ])
    projectile_body += "<p>Example: enemy torpedoes approach; defensive guns fire, countermeasures break some locks, and others strike the shield.</p>"
    projectile_body += "<p>Your offensive guns keep working throughout. Position, equipment and operating modes determine which defenses can help.</p>"
    projectile_body += "<p>Proposed feedback: kinetic shield ripples, explosive blooms, shield failure flashes and hull debris. Effects must leave incoming threats readable.</p>"
    projectile_body += "<p>Exact speeds, damage, recharge, mount compatibility and missile reacquisition remain open. These weapon roles are proposals.</p>"
    projectile_body += "<p>Design records changed; game code remains unchanged. The current instant pulse still needs replacement.</p>"
    projectile_body += "<p>Prior runtime evidence remains historical. This round adds no gameplay or performance claims.</p>"
    projectiles += decide.info_thread("aquila-direction-r25-info",
        "Yes. Your battleship grows through equipment and defenses, while projectile travel gives tactical choices time to matter.",
        body_html=projectile_body, short="Projectile combat and ship growth", follows="aquila-direction-r24-pace")
    projectiles += decide.whats_next("aquila-direction-r25-next",
        "Define the starter shield and first projectile encounter next. Broader upgrades remain design work; no decision is required now.",
        short="Shape the starter encounter", follows="aquila-direction-r24-next")
    rounds = page.entry(25, "Build a battleship over time", projectiles, when="2026-11-15 UTC") + rounds
    cinema = decide.recap(
        "Weapon families are accepted. Even starter guns need visible, satisfying fire. "
        "Autocannons should send readable bullet streams toward targets. "
        "Some defenses must completely stop unsuitable weapons. "
        "Blocked hits should look dramatic while clearly showing no damage. This records design; gameplay remains unchanged.",
        [
            ("Visibility", "Every weapon needs visible firing, travel and impact feedback, including basic starter guns."),
            ("Growth", "Weapon levels can increase spectacle without making early weapons feel invisible or dull."),
            ("Performance", "Representative visible bullets are acceptable; the weapon stream and battle outcomes must remain intact."),
            ("Protection", "Some weapon and defense combinations must cause genuinely zero damage."),
            ("Clarity", "My recommendation separates damage type, penetration and protection, with clear feedback when an attack cannot work."),
        ],
        {
            "did": "Recorded acceptance of the weapon families, cinematic firing requirements and complete protection against unsuitable weapons.",
            "why": "You want visually impressive battles where equipment choices determine whether attacks can work.",
            "working": "The design now distinguishes visible spectacle from damage effectiveness and marks proposed rules separately.",
            "not_working": "These effects and defense rules remain unbuilt. The existing prototype still uses instant pulse hits.",
            "next": "Define a small weapon and defense matchup table alongside the starter encounter.",
        },
    )
    cinema_body = "<p>You accepted the weapon families and added cinematic firing plus complete protection against unsuitable attacks.</p>"
    cinema_body += table(["Your requirement", "Meaning"], [
        ("Visible weapon use", "Even starter guns must look satisfying. Autocannons need a visible stream of traveling bullets or tracers."),
        ("Growing spectacle", "Higher levels can look more powerful. Visible firing must not wait for late-game upgrades."),
        ("Performance allowance", "Showing a representative subset of bullets is acceptable when needed. The stream must still communicate weapon use."),
        ("Complete protection", "An unsuitable weapon can deal exactly zero damage. Ship configuration and equipment choice must matter."),
    ])
    cinema_body += "<p>For our game, your material example means some attacks cannot overcome particular defenses. It does not establish real-world material rules.</p>"
    cinema_body += table(["My visual recommendation", "Result"], [
        ("Show the whole firing action", "Muzzle flashes and recoil lead into traveling rounds, then visible contact or a miss."),
        ("Make autocannons readable", "Separated bright streaks form a moving stream. They never become a continuous instant beam."),
        ("Preserve weapon identity", "Heavy rounds, missiles and torpedoes get distinct launches, trails and impact effects."),
        ("Reduce secondary detail first", "Lower settings cut sparks, smoke and debris before losing weapon streams or incoming threats."),
        ("Keep simulation unchanged", "Fewer drawn rounds never change damage, accuracy or interception. Visual sampling is not missing combat simulation."),
        ("Keep the battle visible", "Spectacle grows through rhythm and richer effects; it must not hide targets or incoming threats."),
    ])
    cinema_body += "<p>My proposed defense model separates attack mechanism, penetration and damage amount. Defenses define compatibility, protection thresholds and remaining resistance.</p>"
    cinema_body += table(["Proposed matchup", "Outcome", "Useful response"], [
        ("Light bullets against armor above their penetration capability", "Zero armor or hull damage", "Use compatible heavy or armor-piercing ammunition"),
        ("Armor-piercing rounds against armor they can penetrate", "Damage can reach the protected ship", "Trade tracking or reload speed for penetration"),
        ("Disruption payload against fully protected electronics", "No disruption effect", "Use another attack mechanism"),
    ])
    cinema_body += "<p>These matchups are examples, not final balance. Protection applies to the struck layer or effect, not automatically the whole ship.</p>"
    cinema_body += "<p>A bullet might drain a shield but fail against the armor beneath it. Those outcomes need different feedback.</p>"
    cinema_body += "<p>Complete protection means no hidden chip damage. More identical bullets cannot secretly wear down an immune layer.</p>"
    cinema_body += "<p>Higher damage or firing rate should not automatically change compatibility. Specific penetration or payload upgrades can change the matchup.</p>"
    cinema_body += "<p>Blocked shots can spark, deflect or ripple across shields. Show No penetration or Blocked instead of suggesting hull damage.</p>"
    cinema_body += "<p>Scanning or observed impacts should expose protection. Automatic weapons can then favor effective equipment and targets.</p>"
    cinema_body += "<p>If no fitted weapon can damage the target, report that clearly. Refitting, another target, allied help or withdrawal remain choices.</p>"
    cinema_body += "<p>Only design records changed. No gameplay or performance claims were added; exact effects and matchup rules still need testing later.</p>"
    cinema += decide.info_thread("aquila-direction-r26-info",
        "Yes. Every weapon should visibly fire, and unsuitable attacks can be completely blocked by the right defense.",
        body_html=cinema_body, short="Cinematic fire and complete protection", follows="aquila-direction-r25-info")
    cinema += decide.whats_next("aquila-direction-r26-next",
        "Define the first weapon and defense matchups alongside the starter encounter. No decision is required now; gameplay remains unchanged.",
        short="Define the first matchups", follows="aquila-direction-r25-next")
    rounds = page.entry(26, "Visible fire, meaningful defenses", cinema, when="2026-11-15 UTC") + rounds
    agreed = decide.recap(
        "The combat vision is agreed and recorded. Cinematic projectile fire and meaningful defense matchups form the foundation. "
        "Exact balance remains open. The prototype is unchanged; the first starter encounter is the next implementation milestone.",
        [
            ("Agreement", "Your confirmation settles the discussed combat vision."),
            ("Weapons", "Automatic projectiles must visibly fire and feel satisfying from the starting level."),
            ("Defenses", "Compatibility and penetration can produce complete protection, with clear blocked-hit feedback."),
            ("Open details", "Exact weapon values, protection thresholds, effects and progression costs still need development."),
            ("Current state", "No gameplay changed. The prototype still uses instant pulse hits."),
        ],
        {
            "did": "Recorded your agreement with the combat vision.",
            "why": "The discussed visual and defense principles are now accepted direction.",
            "working": "The design distinguishes accepted principles from remaining balance and implementation choices.",
            "not_working": "Projectile combat and the new defense model remain unbuilt.",
            "next": "The next implementation milestone is the starter projectile encounter with blocked and penetrating hits.",
        },
    )
    agreed += decide.info_thread("aquila-direction-r27-info",
        "The combat vision is agreed and recorded.",
        body_html="<p>Your direct chat confirms cinematic fire, meaningful defense compatibility and clear feedback when attacks cannot penetrate.</p>"
                  "<p>Damage type, penetration and damage amount remain separate. Defense layers resolve separately; complete protection has no hidden chip damage.</p>"
                  "<p>Automatic weapons favor known effective options. Visual quality changes must preserve combat outcomes.</p>"
                  "<p>Exact balance and effect details remain open. This records agreement; no game code changed or new playtest occurred.</p>",
        short="Combat vision agreed", follows="aquila-direction-r26-info")
    agreed += decide.whats_next("aquila-direction-r27-next",
        "Next implementation milestone: two automatic kinetic turrets, one shield, and readable blocked versus penetrating hits. No decision is required now.",
        short="Starter projectile encounter next", follows="aquila-direction-r26-next")
    rounds = page.entry(27, "Combat vision agreed", agreed, when="2026-11-15 UTC") + rounds
    kinetic = decide.recap(
        "The first projectile encounter is built. Two turrets launch visible rounds; damage waits for impact. "
        "Standard rounds drain the shield but cannot pierce reinforced armor. Fit piercing rounds to finish the fight. "
        "The same battle works from the bridge and outside. Local checks pass.",
        [
            ("Fire", "Two automatic turrets fire staggered bursts, with muzzle flashes, recoil and visible traveling rounds."),
            ("Damage", "Both sides take damage on contact. Moving away from a launched trajectory can avoid a hit."),
            ("Armor", "Standard rounds cause no hull damage against reinforced armor; confirmed blockage stops ineffective new fire."),
            ("Ammunition", "Fit piercing rounds at a station or outside. Rounds already flying retain their original ammunition."),
            ("Checks", "Production build, thirty model tests, six safety tests and nine browser groups pass."),
        ],
        {
            "did": "Replaced instant pulses with the approved two-turret projectile encounter, one shield and blocked versus penetrating hits.",
            "why": "You approved building the first playable version of the agreed combat vision.",
            "working": "Traveling rounds, armor protection, ammunition modes, shared views, pause, recovery, victory and defeat pass local checks.",
            "not_working": "Missiles, automatic evasion and progression remain future work. Physical phones and long sessions remain untested.",
            "next": "The local practice encounter is ready. Further combat systems wait for direction; no decision is required now.",
        },
    )
    kinetic_body = '<p>Choose Start practice in the <a href="http://127.0.0.1:4200/play">local game</a>, or use the bridge tactical console.</p>'
    kinetic_body += table(["Practice choice", "What happens"], [
        ("Standard rounds", "Twin autocannons drain the shield. Reinforced armor blocks hull damage completely."),
        ("Piercing rounds", "Change practice ammunition to penetrate armor. Guns still aim and fire themselves."),
        ("Hold fire", "Stop new launches. Flying rounds and incoming attacks continue."),
        ("Reset practice", "Restore both ships, standard ammunition and automatic fire. The ship and current station stay in place."),
    ])
    kinetic_body += "<p>New firing effects include muzzle flashes, recoil, separate bright rounds, shield ripples, blocked-hit sparks and hull impacts.</p>"
    kinetic_body += "<p>The old instant pulse is removed from both sides of this encounter. Damage occurs only when a traveling round reaches contact.</p>"
    kinetic_body += "<p>Planet obstruction and current ship movement affect projectile contact. Already launched rounds do not steer toward you.</p>"
    kinetic_body += "<p>The first blocked hit identifies the target's armor. New ineffective launches stop until compatible ammunition is fitted.</p>"
    kinetic_body += "<p>These unlimited practice loadouts demonstrate weapon compatibility. They do not grant permanent upgrades or implement ammunition logistics.</p>"
    kinetic_body += "<p>One shield protects each ship. Depletion exposes armor and hull; shield overflow checks armor instead of leaking minimum damage.</p>"
    kinetic_body += "<p>Bridge and exterior share damage, modes and flying rounds. Weapons continue while you walk aboard.</p>"
    kinetic_body += "<p>All quality levels preserve bullet streams. Combat effects reduce secondary sparks without changing collision or damage.</p>"
    kinetic_body += page.figure(page.copy_media(HERE / "media/r28-projectile-encounter.webm", out), "28.1 Projectile encounter",
                                " Native browser recording: standard rounds meet armor, then piercing rounds complete the fight.", look=True)
    kinetic_body += page.figure(page.copy_media(HERE / "media/r28-projectile-exterior-stream.png", out), "28.2 Two visible streams",
                                " Left and right mounts launch separated kinetic rounds. The target shield has not taken damage yet.")
    kinetic_body += page.figure(page.copy_media(HERE / "media/r28-projectile-exterior-blocked.png", out), "28.3 Armor holds",
                                " Standard rounds have depleted the shield. Reinforced armor keeps the hull intact.")
    kinetic_body += page.figure(page.copy_media(HERE / "media/r28-projectile-bridge-stream.png", out), "28.4 Bridge view",
                                " Paused during projectile travel. The same weapon streams are visible through the bridge windows.")
    kinetic_body += page.figure(page.copy_media(HERE / "media/r28-projectile-mobile.png", out), "28.5 Narrow controls",
                                " Ammunition and fire modes fit the narrow desktop viewport. This does not establish physical-phone performance.")
    kinetic_body += "<p>Production build, thirty model tests, six safety tests and nine focused browser groups pass. No browser application errors occurred.</p>"
    kinetic_body += "<p>Checks cover delayed hits, armor immunity, movement misses, ammunition changes, walking, pause, recovery, range, victory, defeat and resource reuse.</p>"
    kinetic_body += "<p>Visual pools stay fixed at 160 rounds and 32 impacts. Repeated encounter resets reused geometry and textures.</p>"
    kinetic_body += "<p>One stationary target remains. Collision uses coarse shapes; turret arcs and own-hull obstruction remain unrestricted.</p>"
    kinetic_body += "<p>Damage stays temporary. Missiles, countermeasures, automatic evasion, progression, sounds and advanced enemy tactics remain future work.</p>"
    kinetic_body += "<p>Physical devices, other browser engines, long play and the desktop runtime remain untested. No website deployment occurred.</p>"
    kinetic += decide.info_thread("aquila-direction-r28-info",
        "Two automatic kinetic turrets now fire traveling rounds, with visible shield hits, blocked armor and penetrating damage.",
        body_html=kinetic_body, short="First projectile encounter built")
    kinetic += decide.whats_next("aquila-direction-r28-next",
        "The local encounter is ready to play. Broader combat systems remain future work; no decision is required now.",
        short="Projectile encounter ready", follows="aquila-direction-r27-next")
    rounds = page.entry(28, "Kinetic rounds take flight", kinetic, when="2026-11-15 UTC") + rounds
    helm = decide.recap(
        "The battleship follows your orders. Choose where to go, hold position, keep combat distance or withdraw. "
        "Its helm steers while you walk aboard. Manual battleship controls are hidden; shuttle piloting remains. "
        "You also confirmed the projectiles work well.",
        [
            ("Orders", "Choose a destination, hold position, keep combat range or withdraw; the helm handles movement."),
            ("Controls", "Manual battleship controls remain underneath but are hidden and cannot steer the ship."),
            ("Aboard", "Navigation continues while you walk. Launching a shuttle first parks the battleship."),
            ("Camera", "Drag either mouse button outside to look around; release returns behind the battleship."),
            ("Checks", "Production build, 35 model tests and 36 browser check groups pass."),
        ],
        {
            "did": "Changed battleship flight to navigation orders while retaining the underlying manual controls.",
            "why": "Your role is to command; the helm handles movement.",
            "working": "Destination travel, range orders, walking, pause, shuttle flight and projectile combat pass local checks.",
            "not_working": "Navigation remains a prototype; advanced tactics and automatic projectile evasion remain unbuilt.",
            "next": "Use navigation orders in the local game. Nothing needs a decision now.",
        },
    )
    helm_body = "<p>Your playtest confirms the preceding projectile encounter works well. The new helm changes are checked locally.</p>"
    helm_body += table(["Order", "Helm response"], [
        ("Approach orbit / Atlas", "Plot a course to the selected world, avoid planetary boundaries and hold at arrival."),
        ("Hold position", "Cancel the current movement order and stop."),
        ("Keep combat range", "Move toward or away from Sentinel to keep 35 km distance."),
        ("Withdraw", "Move out to 85 km, beyond practice firing range. Hold if already farther away."),
    ])
    helm_body += "<p>Combat distances are provisional. Navigation does not change ammunition or firing mode; weapons remain independently automatic.</p>"
    helm_body += "<p>Orders work outside and at consoles. The helm keeps moving while you walk aboard or switch views.</p>"
    helm_body += "<p>Pause, settings and graphics recovery freeze unfinished courses. Resuming continues the order; Hold position cancels it.</p>"
    helm_body += "<p>Launching from the hangar parks the battleship first. Reload restores position with no unfinished order.</p>"
    helm_body += "<p>Manual battleship hints, steering, thrust buttons and flight reticle are hidden. Keyboard and mouse flight inputs cannot move it.</p>"
    helm_body += "<p>Walking and manual shuttle flight remain available. Camera look still returns behind the ship when released.</p>"
    helm_body += page.figure(page.copy_media(HERE / "media/r29-helm-approach.png", out), "29.1 Automatic approach",
                             " The battleship follows an orbit order without manual steering or thrust.", look=True)
    helm_body += page.figure(page.copy_media(HERE / "media/r29-helm-command-console.png", out), "29.2 Command console",
                             " Give orders from inside. Leaving the console does not cancel navigation.")
    helm_body += page.figure(page.copy_media(HERE / "media/r29-helm-mobile-orders.png", out), "29.3 Narrow combat orders",
                             " Movement orders share the existing glass controls. The panel scrolls within the narrow desktop viewport.")
    helm_body += "<p>Production build, 35 model tests and 36 browser check groups pass. No application errors remain in successful runs.</p>"
    helm_body += "<p>Checks cover hidden inputs, camera chords, navigation, walking, launch, combat, pause, graphics recovery, narrow controls and reload.</p>"
    helm_body += "<p>One test initially looked for console controls while walking. Another run was interrupted by my development reload; corrected reruns passed.</p>"
    helm_body += "<p>The helm uses prototype routes and coarse planetary clearance. Advanced tactics, projectile-aware evasion and physical-device validation remain open.</p>"
    helm_body += "<p>No commit, push, deployment, asset transfer or paid generation occurred.</p>"
    helm += decide.info_thread("aquila-direction-r29-info",
        "The battleship now steers and navigates automatically from your orders.",
        body_html=helm_body, short="Automatic helm ready")
    helm += decide.whats_next("aquila-direction-r29-next",
        "The local command controls are ready. Broader tactics and upgrades remain future work; no decision is needed now.",
        short="Command the battleship", follows="aquila-direction-r28-next")
    rounds = page.entry(29, "Command the battleship", helm, when="2026-11-15 UTC") + rounds
    wrapped = decide.recap(
        "The remake is saved, committed and pushed. Gameplay work is paused. "
        "The checkpoint preserves decisions, controls, test evidence and unfinished work. "
        "The local game and report remain running. We can discuss the information you want to share.",
        [
            ("Saved", "Source, tests, design records and report history are committed."),
            ("Pushed", "The source commit is verified on the remote main branch."),
            ("Checks", "Existing build and gameplay evidence still matches; six safety tests passed again."),
            ("Boundaries", "Shared staging was preserved. Media remain local; the website was not deployed."),
            ("Pause", "Further game work waits for your discussion topic."),
        ],
        {
            "did": "Wrapped the session, saved its checkpoint, committed owned work and verified the source push.",
            "why": "You requested a pause and publication before discussing new information.",
            "working": "The local prototype, saved evidence, source publication and restart records are ready.",
            "not_working": "Physical devices, long play, other browser engines and the desktop runtime remain untested.",
            "next": "Wait for your discussion topic. No further gameplay work starts during this pause.",
        },
    )
    wrap_body = table(["Saved work", "Result"], [
        ("Source commit", "<code>9b68aba422b7b2761ec29fbfaa52cd7745a681b1</code>"),
        ("Remote", "Verified on <code>origin/main</code>; ordinary push, with no history rewrite."),
        ("Scope", "Universe, glass interface, shuttles, battleship interiors, projectile combat, automatic helm, tests and session records."),
        ("Shared work", "Private staging held 113 owned paths. Existing shared entries were preserved; no peer staging was present."),
        ("Current evidence", "Production build, 35 model tests and 36 browser groups passed before wrap. Source hashes still match."),
        ("Wrap checks", "Six safety tests passed again. Guard, hook, whitespace and startup links passed."),
        ("Archive", "The preceding checkpoint was preserved byte for byte. Decisions and lessons remain in project records."),
        ("Still running", "Game preview and both local reports remain running. No tests or transfers remain active."),
    ])
    wrap_body += '<p>Saved records: <a href="handoff.md">checkpoint</a>, <a href="restart.txt">restart prompt</a> and <a href="publication.json">publication evidence</a>.</p>'
    wrap_body += "<p>Physical devices, other engines, long play, desktop runtime, fresh agent sessions and actual provider capture remain untested.</p>"
    wrap_body += "<p>Assets, secrets and report media remain outside Git. No website deployment, asset transfer or paid generation occurred.</p>"
    wrapped += decide.info_thread("aquila-direction-r30-info",
        "The remake is committed and pushed; gameplay work is paused for your discussion.",
        body_html=wrap_body, short="Session wrapped and source pushed")
    wrapped += decide.whats_next("aquila-direction-r30-next",
        "Wait for the information you want to discuss. No next gameplay milestone starts during this pause.",
        short="Ready for the discussion", follows="aquila-direction-r29-next")
    rounds = page.entry(30, "Wrapped for discussion", wrapped, when="2026-11-15 UTC") + rounds
    native = decide.recap(
        "Yes. Unreal can support a downloadable game, with the website explaining it and linking to stores. "
        "My recommendation: Steam first, but test phones early. Keep the browser prototype as our design reference. "
        "This needs a rebuild and a smaller first release. No migration has started.",
        [
            ("Game", "Unreal supports Windows, Apple mobile devices and Android through separate platform builds."),
            ("Website", "A marketing website can retain Aquila's identity, ship catalog and optional lightweight 3D showcase."),
            ("Port", "Keep proven gameplay decisions; rebuild the runtime and interface in Unreal."),
            ("Release", "I recommend Steam first, with representative phones tested from the first native slice."),
            ("Scope", "The full vision remains substantial work. Engine choice does not make every planned system quick to build."),
        ],
        {
            "did": "Read your supplied discussion and checked current official platform, branding and AI-disclosure guidance.",
            "why": "You want to discuss downloadable games and a marketing website instead of a full browser game.",
            "working": "Supported platform builds and the existing prototype provide a credible starting point for planning.",
            "not_working": "No Unreal version exists. Device performance, touch controls, release scope and launch order remain unproven or undecided.",
            "next": "Choose a lead release platform, then discuss target devices and the smallest useful native test.",
        },
    )
    native_body = "<p>Your proposal makes sense: a downloadable Unreal game, supported by a website for information and store links.</p>"
    native_body += '<p>Epic documents separate Windows, iOS and Android builds. This supports the plan, not guaranteed performance or store acceptance.</p>'
    native_body += '<p>Source: <a href="https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-your-project">Epic packaging guide</a>.</p>'
    native_body += table(["Part", "Proposed treatment"], [
        ("Browser prototype", "Keep it as the playable reference for camera feel, scale, automatic combat, navigation and interiors."),
        ("Native game", "Rebuild gameplay and interface in Unreal. Existing web code is not a direct export into the engine."),
        ("Website", "Explain the game, show its ships and link to real store listings when available. Keep an optional lightweight 3D showcase."),
        ("First test", "One flagship, one planet backdrop, one interior and one automatic projectile encounter. Measure real devices before adding content."),
    ])
    native_body += "<p>I recommend Steam first, with real iPhone and Android tests from the first playable native slice.</p>"
    native_body += "<p>That avoids coordinating three launches while keeping phone limits visible early. Touch controls, heat, memory and readable effects need testing.</p>"
    native_body += "<p>Automatic helm and weapons seem promising for touch commands. That is my design inference; walking and shuttle controls still need adaptation.</p>"
    native_body += '<p>Phone graphics need their own budget. Standard mobile rendering omits desktop features including Nanite and Lumen.</p>'
    native_body += '<p>Source: <a href="https://dev.epicgames.com/documentation/unreal-engine/rendering-features-reference">Epic mobile rendering matrix</a>.</p>'
    native_body += '<p>Signed iOS builds require a Mac and Apple Developer account. Windows development can use a remote Mac with Xcode.</p>'
    native_body += '<p>Source: <a href="https://dev.epicgames.com/documentation/en-us/unreal-engine/creating-remote-builds-of-unreal-engine-projects-for-ios">Epic remote build guide</a>.</p>'
    native_body += "<p>The complete exploration, crew, progression and planet-activity vision remains substantial work. A smaller first commercial scope needs agreement.</p>"
    native_body += "<p>The AI conversation needs one correction: Valve focuses disclosure on shipped content players consume, not all development-tool efficiency gains.</p>"
    native_body += '<p>Source: <a href="https://partner.steamgames.com/doc/gettingstarted/contentsurvey">Valve content survey</a>. This does not exempt every generated asset or procedural output.</p>'
    native_body += "<p>Automatic navigation and seeded worlds do not inherently require live generative AI. Changing engines does not settle disclosure or content rights.</p>"
    native_body += '<p>Google has separate <a href="https://support.google.com/googleplay/android-developer/answer/17262077?hl=en">promotional-asset declaration guidance</a>. Its treatment of gameplay captures containing AI-created assets remains unresolved here.</p>'
    native_body += '<p>Public studio branding can wait. Valve supports names linking to the same or different controlled <a href="https://partner.steamgames.com/doc/store/creator_homepage">creator homepages</a>.</p>'
    native_body += "<p>Only discussion records and this report changed. No game migration, native build, phone test, store submission or new publication occurred.</p>"
    native += decide.info_thread("aquila-direction-r31-info",
        "Unreal and a separate game website fit the proposed direction; release order and scope remain open.",
        body_html=native_body, short="Native release proposal")
    native_qid = "aquila-direction-r31-release"
    native_choices = table(["Release order", "Tradeoff", "Choose"], [
        ("Steam first", "My recommendation. Test phones early, then ship mobile versions after the first commercial release.",
         decide.pick(native_qid, "steam_first", "Steam first", mode="single", mine=True)),
        ("All three together", "Coordinated launch requires every platform's build, device tests and store review to be ready.",
         decide.pick(native_qid, "together", "All three together", mode="single")),
        ("Mobile first", "Phone interaction and device limits lead the first release; Steam follows.",
         decide.pick(native_qid, "mobile_first", "Mobile first", mode="single")),
    ])
    native += decide.question(native_qid,
        "Which platform should lead the first release?",
        [], short="Release priority", anchors_html=native_choices,
        why="Steam first is my recommendation: fewer simultaneous releases. This preference does not authorize implementation.")
    native += decide.whats_next("aquila-direction-r31-next",
        "Discuss release priority, target devices and first-release scope. Migration remains paused until you request concrete implementation.",
        short="Set the release direction", follows="aquila-direction-r30-next")
    rounds = page.entry(31, "Unreal and native releases", native, when="2026-11-15 UTC") + rounds
    shared = decide.recap(
        "Steam and mobile together becomes the working target. One account carries your single-player progress between devices. "
        "Cloud saves need internet; battles do not need multiplayer servers. "
        "The website keeps its style, with news, development logs and store links instead of a playable game. Implementation remains paused.",
        [
            ("Release", "Aim for a coordinated Steam and mobile launch, with all three platforms developed and tested early."),
            ("Progress", "Link devices to the same game profile and resume the same synchronized save."),
            ("Gameplay", "Single-player only. Online account and save services are separate from multiplayer networking."),
            ("Website", "Keep the space and glass style, subtle 3D, news, development logs and store links. No playable website game."),
            ("Status", "Direction recorded from chat. No native game, account service or website redesign has been built."),
        ],
        {
            "did": "Recorded your coordinated-launch preference, shared-save requirement, single-player scope and website direction.",
            "why": "Progress should follow the player between desktop and phones while the website promotes the game.",
            "working": "The requirements are recorded. Existing services demonstrate shared player storage across platforms and storefronts.",
            "not_working": "Account linking, native saves, cloud sync and mobile performance remain unbuilt or untested.",
            "next": "Discuss the first commercial scope, then plan a small native test that includes shared saves.",
        },
    )
    shared += decide.reply(
        "aquila-direction-r32-release-answer", "aquila-direction-r31-release", "Release priority",
        "it's probably smart to release it on Steam and mobile together, right?",
        "<p>A coordinated launch becomes the working target. All platforms need early development and testing; readiness still determines release dates.</p>"
        "<p>Recorded from your direct chat. No report submission was created.</p>",
        short="Coordinated launch preference",
    )
    shared_body = "<p>Yes. The same single-player campaign can follow you between Steam, iPhone and Android.</p>"
    shared_body += "<p>Multiplayer is excluded from the game scope. Account login and cloud saves still require an online service.</p>"
    shared_body += table(["Proposed player experience", "Behavior"], [
        ("Start on a phone", "Save ship configuration, progress and world state under one game profile."),
        ("Continue on desktop", "Sign in or link Steam to that profile. Download the synchronized save and resume."),
        ("Play offline", "My recommendation: save locally and synchronize when connected. Upload must finish before another device can receive that progress."),
        ("Conflicting progress", "Preserve both versions and explain the choice. Never silently overwrite a different offline campaign."),
        ("Game updates", "Keep compatible save formats across releases. Device graphics and controls stay separate from campaign progress."),
    ])
    shared_body += "<p>Account provider and login methods remain open. A shared profile does not require choosing a separate password flow now.</p>"
    shared_body += '<p>Shared player storage is an existing service category. <a href="https://onlineservices.epicgames.com/player-game-data?lang=en-US">Epic documents storage across platforms and storefronts</a>.</p>'
    shared_body += "<p>That supports feasibility, not a provider selection or tested integration. Storage alone does not implement conflict handling or account linking.</p>"
    shared_body += '<p><a href="https://partner.steamgames.com/doc/features/cloud">Steam Cloud</a> documents Steam-managed synchronization. It does not establish automatic shared saves with App Store and Google Play editions.</p>'
    shared_body += "<p>Shared progress and purchase ownership remain separate decisions. No purchase model has been chosen.</p>"
    shared_body += "<p>The public website will contain no playable game. Keep its current space and glass identity with <code>Three.js</code> presentation.</p>"
    shared_body += "<p>Include game information, news, development notes and links to Steam, the App Store and Google Play.</p>"
    shared_body += "<p>Background gameplay videos are a future website feature; no new footage exists yet.</p>"
    shared_body += "<p>The browser prototype remains a development reference. Coordinated release adds preparation work; it is a target, not a promised date.</p>"
    shared_body += "<p>A future first native test should move one save between desktop and phones, alongside its small battle and interior.</p>"
    shared_body += "<p>Only project records and this report changed. No migration, service setup, website redesign, gameplay testing or new publication occurred.</p>"
    shared += decide.info_thread("aquila-direction-r32-info",
        "Shared saves fit a single-player game, with online synchronization separate from the battles.",
        body_html=shared_body, short="One campaign across devices", follows="aquila-direction-r31-info")
    shared += decide.whats_next("aquila-direction-r32-next",
        "Discuss first-release scope and a native test including shared saves. Implementation remains paused; nothing needs a decision now.",
        short="Define the first release", follows="aquila-direction-r31-next")
    rounds = page.entry(32, "One campaign across devices", shared, when="2026-11-15 UTC") + rounds
    saved = decide.recap(
        "Yes. The discussion was already saved, and your agreement is now recorded. "
        "Project notes, decisions, a restart checkpoint and a native milestone brief preserve it. "
        "Next: check build readiness, then prove a small native battle and shared saves on desktop and phones. "
        "These new records are local and uncommitted.",
        [
            ("Agreement", "Offline local saves, later synchronization and protection of conflicting versions are now accepted requirements."),
            ("Records", "Knowledge, decisions, backlog, checkpoint and report preserve the direction and remaining choices."),
            ("Next milestone", "A small native battle and interior should prove gameplay and save continuity across desktop and phones."),
            ("First step", "Check available build tools, Mac access and actual test devices before creating the native project."),
            ("Publication", "Earlier browser work is pushed. The newer discussion records remain saved locally and uncommitted."),
        ],
        {
            "did": "Saved your agreement, a concrete native milestone brief and an updated restart checkpoint.",
            "why": "The decisions must survive a new session without relying on chat history.",
            "working": "Durable project records separate accepted requirements, proposed implementation and existing browser evidence.",
            "not_working": "No native build or cloud integration exists. Target devices, provider, pricing and commercial scope remain open.",
            "next": "The proposed next step is a native build-readiness review. Implementation remains paused.",
        },
    )
    saved_body = "<p>Your agreement confirms the discussed direction, including offline saves and preservation of conflicting progress.</p>"
    saved_body += "<p>The discussion was already in project files. Your confirmation and the next milestone are now recorded there too.</p>"
    saved_body += table(["Saved record", "Purpose"], [
        ("<code>knowledge/native-release-direction.md</code>", "Accepted direction, earlier alternatives, source evidence and remaining technical choices."),
        ("<code>Docs/DECISIONS.md</code>", "Dated owner decisions and their scope. No silent conversion of suggestions into approvals."),
        ("<code>Docs/Plans/NATIVE_FOUNDATION.md</code>", "Proposed implementation sequence and evidence needed to finish the first native milestone."),
        ("<code>Docs/Handoffs/HANDOFF_2026-11-15_game-direction.md</code>", "Current state, published browser checkpoint, uncommitted discussion work, limits and stop boundary."),
        ("<code>Docs/Handoffs/PROMPT_2026-11-15_native-direction.txt</code>", "Copyable startup prompt for a fresh session. It preserves the pause and points to current records."),
    ])
    saved_body += "<p>The preceding checkpoint is archived byte for byte with its hash. Earlier report rounds and the prior wrap artifacts remain intact.</p>"
    saved_body += table(["Proposed next sequence", "What it establishes"], [
        ("Build readiness", "Check Unreal tooling, Mac access and real test devices. Set device targets and performance budgets."),
        ("Native foundation", "One flagship, planet backdrop, bridge room, enemy and two automatic projectile turrets, with desktop and touch controls."),
        ("Portable local saves", "Restore campaign state after closing the app. Keep device settings separate and handle save versions deliberately."),
        ("Shared save proof", "Select an account/storage service, then prove desktop-to-phone and phone-to-desktop continuity, interrupted uploads and recoverable offline conflicts."),
    ])
    saved_body += "<p>This is a feasibility milestone, not the complete first commercial game. The full release feature list remains open.</p>"
    saved_body += "<p>The website conversion follows stable game identity and usable native gameplay material. No website deployment is authorized.</p>"
    saved_body += "<p>No provider, purchase model or minimum device has been chosen. The browser prototype remains unchanged.</p>"
    saved_body += "<p>New discussion records are saved locally, not committed or pushed. The earlier browser source and wrap remain published.</p>"
    saved += decide.info_thread("aquila-direction-r33-info",
        "Your decisions are saved in project files, and the next native milestone is written down.",
        body_html=saved_body, short="Direction saved and next milestone defined", follows="aquila-direction-r32-info")
    saved += decide.whats_next("aquila-direction-r33-next",
        "Next proposed step: inspect native build readiness. Implementation stays paused; no decision is needed to finish saving these records.",
        short="Native foundation comes next", follows="aquila-direction-r32-next")
    rounds = page.entry(33, "Direction agreed and saved", saved, when="2026-11-15 UTC") + rounds
    foundation = decide.recap(
        "The native Windows game builds and runs. One ship, bridge and automatic battle have local saves. "
        "Five checks pass, including save conflicts. The Android package is built; phone testing comes next. "
        "Art remains a rough blockout. Cloud saves and Apple builds are still ahead.",
        [
            ("Engine", "Aquila uses verified launcher Unreal 5.8.4, separate from DuskForge's source engine."),
            ("Playable", "One ship, planet backdrop, bridge and automatic battle now run in a Windows package."),
            ("Checks", "Five automation groups pass. Packaged relaunch restores the saved battle and retains its earlier revision."),
            ("Android", "Cooked Android package passes content and signature checks. Physical-device installation and gameplay remain pending."),
            ("Limits", "This is a blockout. Full exploration, finished effects, cloud accounts and Apple device proof remain later work."),
        ],
        {
            "did": "Created the native foundation, verified Windows runtime and saves, and built the Android package.",
            "why": "Your approved next step needs native build and save evidence before expanding the game.",
            "working": "Windows packaging, rendered views, automatic combat, save restoration and five automation groups pass.",
            "not_working": "Cloud saves are unbuilt; phone gameplay, native input, iOS and sustained performance remain unverified.",
            "next": "Connect the Galaxy and check installation, controls, suspension and saved progress.",
        },
    )
    foundation_body = "<p>Your approval started local native work. The browser prototype remains the reference; the marketing website conversion comes later.</p>"
    foundation_body += "<p>Verified launcher engine: <code>5.8.4</code>, changelist <code>58902052</code>. DuskForge's source engine and project remain separate.</p>"
    foundation_body += table(["Part", "Current result"], [
        ("Native project", "<code>native/Aquila/</code>. The wrapper checks engine identity before building."),
        ("Command play", "Approach, hold, combat range and withdrawal orders. Weapons acquire and fire automatically."),
        ("Battle", "Two burst turrets and traveling enemy rounds. Standard ammunition drains shields; piercing ammunition defeats reinforced armor."),
        ("Local saves", "Versioned campaign revisions retain position, ship state, world identity, navigation and flying rounds."),
        ("Save conflicts", "Divergent or invalid revisions pause writes and retain files. Conflict-selection controls and cloud transport remain unbuilt."),
        ("Device baseline", "Shared rendering disables Nanite, Lumen, virtual shadow maps and ray tracing. No phone performance claim yet."),
    ])
    foundation_body += page.figure(page.copy_media(HERE / "media/r34-native-exterior-final.png", out), "34.1 Native exterior",
                                  " Windows package, original blockout geometry and glass command strip. Final ship art remains future work.", look=True)
    foundation_body += page.figure(page.copy_media(HERE / "media/r34-native-bridge-final.png", out), "34.2 Separate bridge",
                                  " Human-scale bridge with a live planet backdrop. Capture caught and helped fix a camera coordinate error.")
    foundation_body += page.figure(page.copy_media(HERE / "media/r34-native-combat-final.png", out), "34.3 Automatic battle",
                                  " Standard rounds drain Sentinel's shield but leave its armored hull intact. Effects remain provisional.")
    foundation_body += page.figure(page.copy_media(HERE / "media/r34-native-resumed.png", out), "34.4 Restored battle",
                                  " Relaunch resumes the saved encounter. Continued enemy fire reduces health; the earlier save remains preserved.")
    foundation_body += table(["Evidence", "What it proves"], [
        ("Five automation groups", "Projectile timing, defense compatibility, navigation, migration, interrupted writes and divergent revisions pass model checks."),
        ("Windows package", "Compiled, cooked and archived successfully. Exterior, bridge, battle and restored battle ran and exited cleanly."),
        ("Rendered checks", "The views above were inspected. Scene switches came from development arguments, not mouse or touch input."),
        ("Restoration", "Same campaign, seed and parent revision after relaunch. Battle continues without resetting health; both revisions remain."),
        ("Input limits", "Native window-control service was unavailable. Actual mouse, keyboard and touch interactions remain unchecked."),
        ("Android package", "Native compilation, cooked packaging, embedded content, signature and archive integrity pass. No connected phone was detected."),
        ("Open work", "Galaxy installation, sustained performance, app suspension, cloud linking and Apple builds need further proof."),
    ])
    foundation_body += '<p>Local Android tools follow <a href="https://developer.android.com/tools/sdkmanager">Google\'s command-line setup</a>. Aquila selects them through process-scoped settings.</p>'
    foundation_body += "<p>Turnkey still flags missing Android Studio. The explicit local toolchain builds successfully; no global SDK change was needed.</p>"
    foundation_body += "<p>The first Android archive lacked cooked content. An explicit packaging step fixed it; final content and signature checks pass.</p>"
    foundation_body += "<p>No DuskForge settings or global SDK paths changed. Epic Launcher remains open after your request.</p>"
    foundation_body += '<p>Repeatable commands and limits: <code>native/Aquila/README.md</code>. Detailed evidence: <a href="native-verification.json">native verification receipt</a>.</p>'
    foundation_body += "<p>Source and records remain local and uncommitted. No store submission, website deployment, asset transfer or paid generation occurred.</p>"
    foundation_body += "<p>Report build and HTTP checks pass. The workspace browser host is disconnected, so no visible-tab check was possible.</p>"
    foundation += decide.info_thread("aquila-direction-r34-info",
        "Windows native game runs, with automatic combat and local save restoration.",
        body_html=foundation_body, short="Native foundation and Windows proof", follows="aquila-direction-r33-info")
    foundation += decide.request("aquila-direction-r34-device",
        "Connect your Galaxy by USB, enable USB debugging and accept the computer authorization prompt.",
        "The phone must be connected for installation and device checks. Report when the computer is authorized.",
        short="Connect the Android test device")
    foundation += decide.whats_next("aquila-direction-r34-next",
        "Install and test on the connected Galaxy. Cloud provider choice follows local device proof; iOS waits for Apple access.",
        short="Prove the phone build", follows="aquila-direction-r33-next")
    rounds = page.entry(34, "Native foundation and device preparation", foundation, when="2026-11-16 UTC") + rounds
    phone = decide.recap(
        "Windows sees your Samsung USB connection. Debugging access is still missing. Enable Developer options, turn on USB debugging, "
        "then approve this computer on your unlocked phone. The Android test package is ready.",
        [
            ("Connected", "Windows detects a Samsung USB device without a reported device error."),
            ("Pending", "The Android debugging tool still lists no devices."),
            ("Phone setup", "Enable Developer options through the phone's software information, then turn on USB debugging."),
            ("Authorization", "Accept the computer authorization prompt while the phone is unlocked."),
            ("Next", "Installation and phone checks follow debugging access."),
        ],
        {
            "did": "Checked Windows USB detection and Android debugging access.",
            "why": "Your phone is plugged in and needs development access for the local game build.",
            "working": "Windows sees the Samsung USB connection. The Android package is ready.",
            "not_working": "No phone is available through the debugging tool yet.",
            "next": "Complete the phone settings and computer authorization below.",
        },
    )
    phone += decide.info_thread("aquila-direction-r35-info",
        "Windows sees the Samsung USB connection; debugging access remains pending.",
        body_html="<p>Your chat confirms the cable is connected. Windows reports a Samsung USB device with status OK.</p>"
                  "<p>The Android debugging tool lists no devices. This does not yet distinguish phone settings from a driver issue.</p>"
                  "<p>No game installation or phone gameplay has occurred.</p>",
        short="Phone connection checked", follows="aquila-direction-r34-info")
    phone_steps = table(["Step", "On your Galaxy"], [
        ("1", "Unlock the phone. Open Settings, About phone, then Software information."),
        ("2", "Tap Build number seven times. Enter your phone PIN if asked."),
        ("3", "Return to the main Settings screen. Open Developer options near the bottom."),
        ("4", "Turn on USB debugging and confirm its prompt."),
        ("5", "Accept Allow USB debugging for this computer. If no prompt appears, reconnect the cable while the phone stays unlocked."),
    ])
    phone_steps += '<p>Steps verified against <a href="https://www.samsung.com/us/support/answer/ANS10003430/">Samsung support</a> and '
    phone_steps += '<a href="https://developer.android.com/studio/run/device">Android device setup</a>.</p>'
    phone_steps += "<p>Report when authorization is accepted, or describe any missing or disabled setting.</p>"
    phone += decide.request("aquila-direction-r35-device",
        "Enable USB debugging and authorize this computer on your Galaxy.",
        phone_steps, short="Enable phone debugging", follows="aquila-direction-r34-device")
    phone += decide.whats_next("aquila-direction-r35-next",
        "Check debugging access, then install the prepared game and test on the Galaxy. Waiting for phone authorization.",
        short="Authorize the phone before installation", follows="aquila-direction-r34-next")
    rounds = page.entry(35, "Galaxy USB setup", phone, when="2026-11-16 UTC") + rounds
    blocker = decide.recap(
        "Auto Blocker is preventing USB debugging. Temporarily turn it off under Security and privacy, then enable debugging "
        "and authorize this computer. This reduces some phone protections. Restore Auto Blocker and turn debugging off after testing.",
        [
            ("Cause", "Your phone reports that Auto Blocker prevents USB debugging."),
            ("Setting", "Open Settings, Security and privacy, then Auto Blocker."),
            ("Temporary change", "Turn Auto Blocker off for this development test."),
            ("Debugging", "Enable USB debugging, then accept this computer's authorization prompt."),
            ("After testing", "Turn USB debugging off and restore Auto Blocker."),
        ],
        {
            "did": "Checked Samsung's guidance against the blockage you reported.",
            "why": "The phone's security setting prevents the development connection.",
            "working": "Developer options are available and the Android package is ready.",
            "not_working": "USB debugging remains blocked. No phone installation or gameplay has occurred.",
            "next": "Temporarily change Auto Blocker, enable debugging and authorize this computer.",
        },
    )
    blocker += decide.info_thread("aquila-direction-r36-info",
        "Auto Blocker explains the disabled debugging toggle.",
        body_html="<p>Your chat identifies the phone's displayed restriction. This is owner-reported evidence, not a device capture.</p>"
                  '<p><a href="https://www.samsung.com/us/support/answer/ANS10003636/">Samsung documents</a> that Auto Blocker restricts USB commands and unofficial app installation.</p>'
                  "<p>Turning it off temporarily removes some protections against untrusted apps and USB commands.</p>",
        short="Phone security setting identified", follows="aquila-direction-r35-info")
    blocker_steps = table(["Step", "Action"], [
        ("1", "Open Settings, Security and privacy, then Auto Blocker."),
        ("2", "Turn Auto Blocker off temporarily. Confirm with your phone PIN if asked."),
        ("3", "Return to Settings, Developer options. Turn on USB debugging."),
        ("4", "Accept Allow USB debugging for this computer. Reconnect the cable while unlocked if the prompt does not appear."),
        ("After testing", "Turn USB debugging off and restore Auto Blocker."),
    ])
    blocker_steps += "<p>Use this trusted computer during testing. Report when authorization is accepted, or describe any remaining block.</p>"
    blocker += decide.request("aquila-direction-r36-device",
        "Temporarily turn off Auto Blocker, then enable USB debugging and authorize this computer.",
        blocker_steps, short="Allow the development connection", follows="aquila-direction-r35-device")
    blocker += decide.whats_next("aquila-direction-r36-next",
        "Confirm debugging access before installing the prepared game. Restore the phone's protections when device testing ends.",
        short="Authorize, test, then restore protection", follows="aquila-direction-r35-next")
    rounds = page.entry(36, "Auto Blocker and USB debugging", blocker, when="2026-11-16 UTC") + rounds
    galaxy = decide.recap(
        "Aquila runs on your Galaxy. Controls, combat and saved progress pass initial device checks. "
        "Phone testing caught an ammunition bug; the installed update fixes it. Six automated checks pass. "
        "Fresh practice is paused for you. Art remains rough, and long-session performance still needs testing.",
        [
            ("Installed", "The updated game is installed and its package hash matches the verified local build."),
            ("Controls", "Computer-sent touches exercised navigation, bridge movement, camera look, combat modes and pause on the phone."),
            ("Saves", "Backgrounding, relaunch and the app update preserved campaign state and earlier save revisions."),
            ("Combat fix", "Old standard bullets no longer stop newly selected piercing weapons. Six automation groups and phone battle checks pass."),
            ("Your turn", "Resume fresh practice and check touch comfort. Finished art, sustained performance and shared cloud saves remain ahead."),
        ],
        {
            "did": "Installed the game, checked phone controls and saves, fixed ammunition switching, then verified the update.",
            "why": "A real phone exposes issues that desktop builds and model checks miss.",
            "working": "Initial phone rendering, injected touches, combat, local restoration, update preservation and six automated checks pass.",
            "not_working": "Human touch comfort, sustained performance, cloud accounts and Apple device proof remain unverified or unbuilt.",
            "next": "Try the paused practice battle and bridge. Restore phone protection when USB testing ends.",
        },
    )
    galaxy_body = "<p>Your debugging connection is authorized. The phone locked during rebuilding; your unlock allowed the final checks to finish.</p>"
    galaxy_body += table(["Check", "Observed result"], [
        ("Phone", "Samsung model <code>SM-S948U</code>, Android 17, API 37, arm64. No retail-model assumption."),
        ("Installation", "Initial installation and update pass. Installed package hash matches the local verified package."),
        ("Controls", "Injected taps and gestures moved the ship, entered the bridge, walked, looked around, changed ammunition and paused."),
        ("Background and relaunch", "Campaign state remained exact, including ship position, health, ammunition and 16 flying rounds. Earlier revisions remained unchanged."),
        ("Update", "All 15 pre-update save revisions survived. The updated app restored the same campaign state."),
        ("Ammunition fix", "Late standard impacts stopped compatible piercing fire. Current ammunition now governs firing; old rounds still bounce off armor."),
        ("Regression proof", "Six model groups pass. Both the restored stuck battle and a fresh mid-battle ammunition switch defeat the target."),
        ("Windows", "Updated package builds and its automatic-battle runtime exits cleanly. Physical desktop input remains unchecked."),
        ("Limits", "These are short device checks. Human touch comfort, multitouch, sustained frame times, heat and battery remain unverified."),
    ])
    galaxy_body += page.figure(page.copy_media(HERE / "media/r37-android-ready.png", out), "37.1 Galaxy ready to play",
                               " Updated phone build. Fresh practice is paused; ship and enemy health reset. Art remains a rough blockout.", look=True)
    galaxy_body += page.figure(page.copy_media(HERE / "media/r37-android-final-bridge.png", out), "37.2 Bridge on the phone",
                               " Bridge view and movement controls render on the Galaxy. Computer-sent touches tested walking and looking around.")
    galaxy_body += page.figure(page.copy_media(HERE / "media/r37-android-fixed-victory.png", out), "37.3 Ammunition switch verified",
                               " Piercing fire defeats the target after switching while older rounds remain in flight.")
    galaxy_body += '<p>Exact device evidence: <a href="android-device-verification.json">Android verification receipt</a>. Overall evidence: <a href="native-verification.json">native foundation receipt</a>.</p>'
    galaxy_body += "<p>Phone media remain local and ignored. Source and records remain uncommitted; no store submission or website deployment occurred.</p>"
    galaxy += decide.info_thread("aquila-direction-r37-info",
        "Aquila is installed and tested on your Galaxy, with the ammunition fix included.",
        body_html=galaxy_body, short="Galaxy installation and initial checks pass", follows="aquila-direction-r36-info")
    galaxy_steps = table(["Try", "Action"], [
        ("Resume", "Tap Pause / resume to start the fresh practice battle."),
        ("Weapons", "Tap Standard / piercing rounds when armor blocks standard fire. Aiming and firing remain automatic."),
        ("Bridge", "Tap Bridge / exterior. Hold a movement button and drag open space to look around."),
        ("Feedback", "Report tiny text, awkward buttons, stutters or unexpected camera movement."),
        ("Phone protection", "When USB testing is finished, turn USB debugging off and restore Auto Blocker."),
    ])
    galaxy += decide.request("aquila-direction-r37-play",
        "Try the practice battle and bridge on your phone, then report how the controls feel.",
        galaxy_steps, short="Try the phone build")
    galaxy += decide.whats_next("aquila-direction-r37-next",
        "Review your touch feedback, then measure sustained phone performance. Cloud provider choice and Apple access remain open.",
        short="Phone usability and sustained performance", follows="aquila-direction-r36-next")
    rounds = page.entry(37, "Aquila running on your Galaxy", galaxy, when="2026-11-16 UTC") + rounds
    phone_ui = decide.recap(
        "Phone controls now have larger bold text and bigger buttons. Navigation, Weapons and Voyage tabs reduce clutter. "
        "Both packages build, and desktop previews pass. The new version is not on your phone yet: "
        "Windows sees the cable, but USB debugging remains unavailable.",
        [
            ("Text", "Phone labels are larger, bold and brighter, with dark shadows for contrast."),
            ("Menus", "Navigation, Weapons and Voyage tabs show one command group at a time."),
            ("View", "Hide menu clears the scene. Bridge and Pause stay available at the top."),
            ("Checks", "Windows and Android packages pass. Both desktop and phone layouts render correctly in desktop previews."),
            ("Pending", "The phone update waits for USB debugging access; actual phone readability still needs your check."),
        ],
        {
            "did": "Enlarged phone controls and grouped commands into three tabs.",
            "why": "Your first playtest found some phone menus small and hard to see.",
            "working": "Both packages build. Larger phone controls fit in the desktop preview.",
            "not_working": "USB debugging lists no phone. The new menu has not been installed or tested there.",
            "next": "Restore debugging access, install the update, then check reading and tapping comfort.",
        },
    )
    phone_ui_body = "<p>Your feedback identifies a readability problem despite successful earlier touch checks.</p>"
    phone_ui_body += table(["Change", "Result"], [
        ("Readability", "Larger bold labels, brighter text and dark shadows over the glass controls."),
        ("Touch targets", "Bigger buttons; one command group visible at a time. Top controls remain available when the menu hides."),
        ("Camera", "Touch camera input excludes visible control panels, including the larger movement buttons."),
        ("Builds", "Updated editor, Windows package and Android package pass. Android content, signature and archive integrity checks pass."),
        ("Rendered checks", "Desktop and phone layouts render at 1600 by 900 pixels in packaged Windows runs. Both exit cleanly."),
        ("Device access", "Your chat confirms reconnection. Windows sees Samsung USB, but debugging still lists no phone."),
        ("Limits", "This update is not installed on the Galaxy. Actual phone layout, touch input and reading comfort remain unchecked."),
    ])
    phone_ui_body += page.figure(page.copy_media(HERE / "media/r38-packaged-touch.png", out), "38.1 Larger phone layout",
                                 " Windows-rendered preview of the phone layout. Galaxy installation waits for debugging access.", look=True)
    phone_ui_body += '<p>Exact checks and remaining gaps: <a href="mobile-ui-verification.json">phone menu verification receipt</a>.</p>'
    phone_ui_body += "<p>The earlier phone build and saves remain in place. Source and records remain local and uncommitted.</p>"
    phone_ui += decide.info_thread("aquila-direction-r38-info",
        "Larger phone menus are packaged; installation waits for debugging access.",
        body_html=phone_ui_body, short="Larger phone controls built", follows="aquila-direction-r37-info")
    phone_ui += decide.request("aquila-direction-r38-device",
        "Check USB debugging is on, accept any authorization prompt, then leave Aquila unlocked.",
        "Windows sees the cable, but debugging lists no phone. Report whether debugging is on or a prompt appears.",
        short="Restore phone debugging access", follows="aquila-direction-r37-play")
    phone_ui += decide.whats_next("aquila-direction-r38-next",
        "Install with saved progress preserved once debugging returns. Then inspect the phone menu and check your reading comfort.",
        short="Install and check the larger menu", follows="aquila-direction-r37-next")
    rounds = page.entry(38, "Larger phone menus", phone_ui, when="2026-11-16 UTC") + rounds
    wake_check = decide.recap(
        "The larger menus are installed on your phone, and you confirmed they feel good. "
        "Two clipped labels are fixed. All 58 earlier save revisions survived the final update. "
        "Menu tabs, bridge controls, saving and camera checks pass.",
        [
            ("Installed", "Refreshing USB debugging restored access. The final phone package is installed and its hash matches."),
            ("Readable", "Larger text and buttons fit. Wider movement buttons and a shorter Menu label fix clipping."),
            ("Saves", "All 58 earlier revisions survived the final update; restored campaign state matched exactly."),
            ("Controls", "Tabs, menu hiding, bridge entry, walking, saving and camera checks pass on the phone."),
            ("Your check", "You confirmed the larger labels and buttons feel good. Sustained performance and multitouch remain unverified."),
        ],
        {
            "did": "Installed the larger menus, fixed clipped labels and checked the final version on your Galaxy.",
            "why": "Your first playtest found menu text too small.",
            "working": "The phone update, saved progress, command tabs and camera checks pass. You confirmed reading and tapping comfort.",
            "not_working": "Multitouch and sustained performance remain unverified. Shared cloud saves and Apple proof remain unfinished.",
            "next": "Readability work is complete. Sustained phone testing remains the next verification step.",
        },
    )
    wake_body = "<p>Your debugging toggle and reconnection restored access. The final update is installed.</p>"
    wake_body += table(["Check", "Observed result"], [
        ("Labels", "Widened movement buttons to show Forward fully. Changed the hidden-menu button to Menu, avoiding clipping."),
        ("Builds", "Updated editor, Windows package and Android package pass. Android signature and archive checks pass."),
        ("Saved progress", "All 58 pre-update revisions remained byte-identical. The same campaign restored before further input checks."),
        ("Phone input", "Injected taps exercised all three tabs, saving, hiding/showing menus, bridge entry, walking and pause/resume."),
        ("Camera", "A swipe over the visible menu left the view steady. The same area rotated the camera after hiding menus."),
        ("Verification limits", "Camera checks used active gameplay. Earlier paused checks were inconclusive. Multitouch, prolonged performance and heat remain unverified."),
    ])
    wake_body += page.figure(page.copy_media(HERE / "media/r39-final-ready.png", out), "39.1 Larger menus on your Galaxy",
                             " Final installed version at 2340 by 1080 pixels. Navigation controls fit; game left paused after checks.", look=True)
    wake_body += page.figure(page.copy_media(HERE / "media/r39-final-bridge.png", out), "39.2 Bridge movement controls",
                             " Forward now fits fully. Movement controls remain separate from the command tabs.")
    wake_body += page.figure(page.copy_media(HERE / "media/r39-final-weapons.png", out), "39.3 Weapons tab",
                             " Larger weapon controls and battle status on the actual phone.")
    wake_body += page.figure(page.copy_media(HERE / "media/r39-final-voyage.png", out), "39.4 Voyage tab",
                             " Saving works locally. Cloud saves remain unbuilt.")
    wake_body += "<p>You confirmed reading and tapping comfort in chat:</p><blockquote>Yes, they feel good</blockquote>"
    wake_body += '<p>Exact checks and remaining gaps: <a href="mobile-ui-verification.json">phone menu verification receipt</a>.</p>'
    wake_check += decide.info_thread("aquila-direction-r39-info",
        "Larger phone menus are installed, tested and confirmed comfortable by you.",
        body_html=wake_body, short="Phone readability update complete", follows="aquila-direction-r38-info")
    wake_check += decide.whats_next("aquila-direction-r39-next",
        "Readability update complete. Next verification work: multitouch and sustained phone performance; cloud sync and Apple proof remain later.",
        short="Phone readability verified", follows="aquila-direction-r38-next")
    rounds = page.entry(39, "Larger phone menus verified", wake_check, when="2026-11-16 UTC") + rounds
    shared_saves = decide.recap(
        "Next recommendation: one campaign shared between Windows and Android. Local saves work; account login and cloud sync are missing. "
        "Choose the service, add safe syncing, then prove you can continue on either device. Your phone can stay unplugged during planning.",
        [
            ("Next", "Finish the shared-save foundation before expanding the combat scene."),
            ("Account", "Choose a service that fits one account across Steam and mobile."),
            ("Offline", "Keep local play and saving available without a connection."),
            ("Protection", "Retain both versions when devices make different offline progress; provide a clear recovery choice."),
            ("Proof", "Continue the same campaign on Windows, then Android, then Windows again."),
        ],
        {
            "did": "Reviewed the accepted milestone and its remaining work to recommend the next step.",
            "why": "Shared progress is an accepted requirement that local saving alone does not fulfill.",
            "working": "Native Windows and Android builds, local saves, initial phone controls and accepted menu readability.",
            "not_working": "Account login and cloud sync are unbuilt. Sustained performance, multitouch and Apple device proof remain open.",
            "next": "Compare account and save services, then develop the chosen integration and verify a device round trip.",
        },
    )
    shared_body = table(["Order", "Proposed work", "Done when"], [
        ("1", "Choose the account and save service.", "Steam/mobile linking, ownership, offline behavior, conflict recovery and cost have a concrete plan."),
        ("2", "Connect the existing local save journal to login and sync.", "Progress saves offline and syncs after reconnection, with visible status."),
        ("3", "Add recovery choices for conflicting progress.", "Both versions survive; neither silently overwrites the other."),
        ("4", "Prove a Windows, Android, Windows round trip.", "The same campaign continues correctly, including interrupted connections and offline divergence."),
    ])
    shared_body += "<p>This follows the accepted foundation plan. Service selection remains open; this reply proposes the next work.</p>"
    shared_body += "<p>Your phone can stay unplugged during planning and initial development. Physical-device sync testing comes afterward.</p>"
    shared_body += "<p>Multitouch and sustained phone performance remain separate checks. Apple testing still needs Mac and iPhone access.</p>"
    shared_saves += decide.info_thread("aquila-direction-r40-info",
        "Next recommendation: finish shared progress between Windows and Android.",
        body_html=shared_body, short="Shared saves proposed next", follows="aquila-direction-r39-info")
    shared_saves += decide.whats_next("aquila-direction-r40-next",
        "First, compare account and save services against the agreed requirements. Choose the service before integration or provisioning.",
        short="Choose the account and save service", follows="aquila-direction-r39-next")
    rounds = page.entry(40, "Next: shared saves", shared_saves, when="2026-11-16 UTC") + rounds
    business = decide.recap(
        "My recommendation: one business and one studio for both games initially. Each game keeps its own identity. "
        "Steam allows separate public studio labels if you later want them. Settle the legal owner before signup. "
        "Player login and shared saves remain separate work.",
        [
            ("Business", "One legal owner and Steamworks partner account can cover both games."),
            ("Studio", "Start with one public studio brand. Different game sizes and genres alone do not require two."),
            ("Options", "Separate labels remain useful if you want distinct audiences and public identities."),
            ("Order", "If forming a company, do that before Steamworks onboarding so legal, bank and tax details match."),
            ("Saves", "A business account does not build player login or shared saves. That foundation remains separate work."),
        ],
        {
            "did": "Checked Valve guidance and separated legal ownership, studio branding and player accounts.",
            "why": "You raised business setup and one versus two studios before choosing the shared-save service.",
            "working": "Valve supports several games per partner account and flexible public developer names and pages.",
            "not_working": "Existing business, location, legal form and studio names remain unknown or undecided. No accounts opened.",
            "next": "Confirm existing business and location, then plan the ownership and signup sequence.",
        },
    )
    business_body = table(["Decision", "Recommendation", "Basis"], [
        ("Legal owner", "One business owner for both games, subject to your circumstances.",
         '<a href="https://partner.steamgames.com/doc/gettingstarted/managing_apps">Valve supports multiple applications per partner account.</a>'),
        ("Public studio", "One brand initially, with distinct identities for Aquila and DuskForge.",
         "Branding judgment: a smaller polished game can build trust. Audience separation may justify a second label."),
        ("Brand flexibility", "Two public names remain possible without choosing two companies now.",
         '<a href="https://partner.steamgames.com/doc/store/creator_homepage">Valve allows multiple names and controlled creator pages.</a>'),
        ("Onboarding", "Choose the legal owner first. Form the company before signup if that is your chosen structure.",
         '<a href="https://partner.steamgames.com/doc/gettingstarted/faq">Valve requires legal identity matching banking and tax documents.</a>'),
        ("Player accounts", "Continue planning shared saves separately from business registration.",
         "Business onboarding does not supply the game integration. Cloud provider and login method remain open."),
    ])
    business_body += "<p>This is a recommendation, not an accepted business structure or a prediction about sales or Steam rankings.</p>"
    business_body += "<p>Official guidance checked today. No company registered, accounts opened, agreements accepted or fees paid.</p>"
    business += decide.info_thread("aquila-direction-r41-info",
        "One business and one studio initially is my recommendation for Aquila and DuskForge.",
        body_html=business_body, short="Business and studio recommendation", follows="aquila-direction-r40-info")
    business += decide.question("aquila-direction-r41-business",
        "Do you already have a registered business, and which country and state would the game business operate from?", [],
        short="Existing business and location",
        why="These facts determine the next setup steps. Entity-specific advice needs your jurisdiction.")
    business += decide.whats_next("aquila-direction-r41-next",
        "Use your business and location details to plan ownership and signup. Studio count remains your branding decision.",
        short="Plan ownership before signup", follows="aquila-direction-r40-next")
    rounds = page.entry(41, "Business and studio identity", business, when="2026-11-16 UTC") + rounds
    texas = decide.recap(
        "You have no business yet, live in Texas and will be the only owner. "
        "My recommendation for review: one Texas single-member LLC for both games. "
        "Choose a name, form the company, obtain its tax ID and bank account, then join Steamworks. "
        "Nothing filed; the structure remains a proposal.",
        [
            ("Confirmed", "Texas, no existing business, and you as sole owner."),
            ("Structure", "Review one Texas single-member LLC for both games with a Texas attorney and CPA."),
            ("Formation", "Standard state filing fee is $300. Agent, professional and payment fees may add costs."),
            ("Sequence", "Company first, then free IRS tax ID, business bank account and Steamworks onboarding."),
            ("Upkeep", "Texas LLCs generally owe an annual Public Information Report even without franchise tax due."),
        ],
        {
            "did": "Saved your location and sole ownership, then checked Texas, IRS and Valve guidance.",
            "why": "Those facts make the business setup discussion concrete.",
            "working": "A Texas LLC is available; the standard filing fee and setup order have official support.",
            "not_working": "Legal structure and studio name remain unchosen. No company formed or platform account opened.",
            "next": "Discuss a company and studio name broad enough for both games, then check availability.",
        },
    )
    texas_body = "<p>You confirmed Texas, no existing business and sole ownership in chat.</p>"
    texas_body += "<p>My proposed structure: one Texas single-member LLC for both games, with one public studio initially.</p>"
    texas_body += "<p>Have a Texas attorney and CPA confirm legal and tax fit before filing.</p>"
    texas_body += table(["Order", "Proposed step", "Basis"], [
        ("1", "Choose a broad company/studio name; check availability and conflicting marks.",
         "One company can support distinct game identities. Studio count remains a branding choice."),
        ("2", "Choose the registered agent and address, review company documents, then file formation.",
         '<a href="https://www.sos.state.tx.us/corp/instructions/205.shtml">Texas standard formation fee: $300; other costs may apply.</a>'),
        ("3", "Obtain the company tax ID directly from the IRS after state formation.",
         '<a href="https://www.irs.gov/businesses/small-businesses-self-employed/get-an-employer-identification-number">IRS tax IDs are free; company formation comes first.</a>'),
        ("4", "Establish the business bank account and document game ownership or publishing rights, then complete Steamworks onboarding.",
         '<a href="https://partner.steamgames.com/doc/gettingstarted/faq">Valve requires the legal publisher identity to match banking and tax records.</a>'),
    ])
    texas_body += '<p><a href="https://comptroller.texas.gov/taxes/franchise/pir-oir-filing-req.php">Texas LLCs generally file an annual Public Information Report, even without franchise tax due.</a></p>'
    texas_body += "<p>Steam does not require an LLC. This recommendation considers your longer-term studio plan; it does not guarantee liability protection.</p>"
    texas_body += "<p>Texas formation claims use official search passages; direct state page opens failed. IRS, Comptroller and Valve pages were readable.</p>"
    texas_body += "<p>Nothing filed, paid or signed. The structure remains a proposal; no name or service has been chosen.</p>"
    texas += decide.info_thread("aquila-direction-r42-info",
        "Texas and sole ownership confirmed; one single-member LLC is my proposed starting structure.",
        body_html=texas_body, short="Texas sole-owner setup plan", follows="aquila-direction-r41-info")
    texas += decide.whats_next("aquila-direction-r42-next",
        "Discuss a company/studio name broad enough for both games. Formation preparation follows ownership, name and structure choices.",
        short="Choose a broad studio name", follows="aquila-direction-r41-next")
    rounds = page.entry(42, "Texas studio setup plan", texas, when="2026-11-16 UTC") + rounds
    individual = decide.recap(
        "No LLC or corporation required. Steam supports individual publishers, and Google Play personal accounts can publish and earn money. "
        "Developer signup and verification still apply. Google directs business publishers to organization accounts. "
        "My LLC suggestion was optional; company formation need not block development.",
        [
            ("Answer", "An LLC or corporation is not required for individual game publishing."),
            ("Steam", "Register as an individual or sole proprietor, using your legal name and matching bank and tax details."),
            ("Google Play", "Personal developer accounts can publish games and earn money."),
            ("Account fit", "Google directs business and organization publishers to organization accounts. Choose the category that matches the actual publisher."),
            ("Earlier advice", "The Texas LLC was an optional structure to evaluate, not a mandatory development step."),
        ],
        {
            "did": "Checked the stores' individual publishing rules and clarified the earlier LLC recommendation.",
            "why": "You asked whether business formation is required before making and publishing the game.",
            "working": "Official Steam and Google guidance supports individual publishing without an LLC or corporation.",
            "not_working": "Publishing identity and account type remain undecided. No signup or formation has started.",
            "next": "Company formation need not block development. Choose the actual publishing identity before store signup.",
        },
    )
    individual_body = table(["Store", "Individual route", "Official guidance"], [
        ("Steam", "An individual content owner can select Sole Proprietorship and supply their legal name, bank and tax details.",
         '<a href="https://partner.steamgames.com/doc/gettingstarted/onboarding">Steam onboarding</a>'),
        ("Google Play", "Personal developer accounts can publish and monetize. Google directs accounts representing businesses to the organization category.",
         '<a href="https://support.google.com/googleplay/android-developer/answer/13634885?hl=en">Google developer account types</a>'),
    ])
    individual_body += "<p>You still need developer registration, identity verification and applicable payment and tax details.</p>"
    individual_body += "<p>Individual publishing does not remove applicable legal or tax duties. My LLC suggestion was optional planning advice.</p>"
    individual_body += "<p>Official guidance checked today. No business formed or accounts opened; current local development need not wait for formation.</p>"
    individual += decide.info_thread("aquila-direction-r43-info",
        "No. An LLC or corporation is not required for the stores' individual publishing routes.",
        body_html=individual_body, short="Company formation is optional", follows="aquila-direction-r42-info")
    individual += decide.whats_next("aquila-direction-r43-next",
        "Company formation remains optional. Publishing identity and the correct account category remain open before signup.",
        short="Choose publishing identity before signup", follows="aquila-direction-r42-next")
    rounds = page.entry(43, "Company formation is optional", individual, when="2026-11-16 UTC") + rounds
    studio_name = decide.recap(
        "Yes. Steam's public Developer and Publisher labels can show your studio name. "
        "Valve still needs your legal identity for onboarding. Google Play also permits a studio display name, "
        "but personal accounts publicly show your legal name. Monetizing adds your full address. "
        "No name or account type chosen.",
        [
            ("Steam label", "Your public Developer and Publisher labels can use a studio name."),
            ("Legal owner", "Individual Steam onboarding still needs your real name and matching bank and tax details."),
            ("Google label", "Google Play also allows a developer display name different from your legal name."),
            ("Public identity", "Google personal accounts disclose legal name, country and developer email. Monetizing also discloses the full legal address."),
            ("Choice", "No studio name, business structure or publishing account selected."),
        ],
        {
            "did": "Checked public studio labels and the stores' legal identity rules.",
            "why": "You asked whether players must see your personal name instead of a studio name.",
            "working": "Both stores permit studio display names. Steam sets these separately from legal onboarding details.",
            "not_working": "A studio label alone does not hide personal-account identity on Google Play.",
            "next": "Include public identity requirements when choosing the publishing account. No registration started.",
        },
    )
    studio_body = table(["Store", "Public name", "Legal identity"], [
        ("Steam", '<a href="https://partner.steamgames.com/doc/store/creator_homepage">Developer and Publisher fields can show your studio brand.</a>',
         '<a href="https://partner.steamgames.com/doc/gettingstarted/onboarding">Individual onboarding requires your real legal name and matching bank and tax details.</a>'),
        ("Google Play", "A studio display name can differ from your legal name.",
         '<a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Personal accounts publicly show legal name, country and developer email.</a>'),
    ])
    studio_body += "<p>Google also displays the full legal address when monetizing. Additional regional disclosures can apply.</p>"
    studio_body += "<p>These are display-name rules, not a promise of anonymity. Official store guidance checked today.</p>"
    studio_body += "<p>No name selected or checked for conflicts. No business formed, account opened or fee paid.</p>"
    studio_name += decide.info_thread("aquila-direction-r44-info",
        "Yes. Steam can show a studio name in its public Developer and Publisher labels.",
        body_html=studio_body, short="Studio name and legal identity", follows="aquila-direction-r43-info")
    studio_name += decide.whats_next("aquila-direction-r44-next",
        "Studio name and publishing identity remain open. Account choice must account for public identity disclosures.",
        short="Choose branding and publishing identity", follows="aquila-direction-r43-next")
    rounds = page.entry(44, "Public studio name", studio_name, when="2026-11-16 UTC") + rounds
    sole_owner = decide.recap(
        "Your friends' experience fits the rules. You can sell games without forming an LLC. "
        "A sole proprietor runs a business as themselves. Steam accepts individuals; Google personal accounts can earn money. "
        "I pushed company setup too early. No reliable developer-wide count checked, and no publishing route chosen.",
        [
            ("Meaning", "A sole proprietor runs the business personally, without forming a separate company."),
            ("Steam", "Individual publishers are allowed."),
            ("Google Play", "Personal developer accounts can earn money. Account category must match the actual publisher."),
            ("Research", "Official rules settle eligibility. They do not show which structure most game developers choose."),
            ("Correction", "The earlier LLC recommendation was premature. No company formation or account route selected."),
        ],
        {
            "did": "Checked official platform rules and the meaning of sole proprietorship.",
            "why": "You know developers who sell without forming companies and questioned the LLC recommendation.",
            "working": "Steam accepts individuals. Google personal accounts can monetize. Company formation is not a universal requirement.",
            "not_working": "No representative data shows what most game developers choose. Your publishing account remains undecided.",
            "next": "Keep company formation optional. Discuss publishing identity before signup; local development need not wait.",
        },
    )
    sole_body = "<p>I gave the LLC option too much weight. Your friends' experience fits the published rules.</p>"
    sole_body += '<p><a href="https://www.sba.gov/counseling/launch-your-business/">A sole proprietor can operate personally without forming a separate business entity.</a></p>'
    sole_body += table(["Store", "Official individual route"], [
        ("Steam", '<a href="https://partner.steamgames.com/doc/gettingstarted/onboarding">Individual content owners can onboard as sole proprietors.</a>'),
        ("Google Play", '<a href="https://support.google.com/googleplay/android-developer/answer/13634885?hl=en">Personal accounts can earn money. Google directs business and organization publishers to organization accounts.</a>'),
    ])
    sole_body += "<p>A focused official-policy check answers the LLC requirement question. A broad research project is not needed for that.</p>"
    sole_body += "<p>I do not have reliable statistics showing which structure most game developers choose.</p>"
    sole_body += '<p><a href="https://www.sos.state.tx.us/corp/businessstructure.shtml">Texas sole proprietors using an assumed business name generally need a county assumed-name filing.</a></p>'
    sole_body += "<p>Texas evidence came from its official search passage; direct page access failed. SBA and Google pages were readable.</p>"
    sole_body += "<p>Applicable taxes, verification and public identity rules still apply. Nothing filed, paid or signed.</p>"
    sole_owner += decide.info_thread("aquila-direction-r45-info",
        "You can sell games without forming an LLC. Operating as an individual is a supported route.",
        body_html=sole_body, short="Selling without company formation", follows="aquila-direction-r44-info")
    sole_owner += decide.whats_next("aquila-direction-r45-next",
        "Publishing identity remains open before signup. Company formation is not a prerequisite for further local game development.",
        short="Keep company formation optional", follows="aquila-direction-r44-next")
    rounds = page.entry(45, "Selling as an individual", sole_owner, when="2026-11-16 UTC") + rounds
    address_privacy = decide.recap(
        "Your home address must stay off public store listings. Google publishes legal addresses for paid apps and in-app purchases. "
        "An LLC alone does not hide an address. A separate business address needs verification; no mailbox service is confirmed. "
        "Google Play signup waits for a solution that meets your privacy requirement.",
        [
            ("Requirement", "Keep your home address off public store listings."),
            ("Google rule", "Paid apps and in-app purchases require a public legal address from the linked payments profile."),
            ("Company", "Organization accounts also display their legal address. An LLC alone does not solve this."),
            ("Possible route", "Investigate a legitimate separate business address. No virtual mailbox or PO box has been verified as acceptable."),
            ("Current state", "No account opened or address service bought. Google Play publishing depends on resolving privacy."),
        ],
        {
            "did": "Saved your home-address privacy requirement and checked Google's address verification rules.",
            "why": "You said public disclosure of your address is unacceptable.",
            "working": "The privacy requirement is recorded. Local Android development does not require a public store listing.",
            "not_working": "No separate address or provider has been verified as acceptable for your publishing setup.",
            "next": "Resolve the public address before Google Play signup. No company formation or address purchase authorized.",
        },
    )
    address_body = "<p>Your home address must stay off public store listings. This is now a publishing requirement.</p>"
    address_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/13634081?hl=en">Google requires merchants to display the full legal address from their linked payments profile.</a></p>'
    address_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Organization accounts also display their legal address.</a> Forming an LLC using your home address would not solve that.</p>'
    address_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/15633622?hl=en">Verification documents must match the name and address in the payments profile.</a></p>'
    address_body += "<p>A legitimate separate business address is a route to investigate. No PO box or virtual office is confirmed acceptable.</p>"
    address_body += "<p>Community suggestions are not a Google acceptance guarantee. No address provider selected or paid.</p>"
    address_body += "<p>Merchant and US verification pages were checked today. Organization disclosure uses the earlier direct read; today's retries timed out.</p>"
    address_body += "<p>Google Play remains a target with an unresolved privacy dependency. No platform canceled or account opened.</p>"
    address_privacy += decide.info_thread("aquila-direction-r46-info",
        "We will not publish your home address. Google Play signup waits for an acceptable privacy solution.",
        body_html=address_body, short="Home address must stay private", follows="aquila-direction-r45-info")
    address_privacy += decide.whats_next("aquila-direction-r46-next",
        "Verify a legitimate separate address before signup or purchase. If none meets your requirement, revisit Google Play distribution.",
        short="Resolve public address before signup", follows="aquila-direction-r45-next")
    rounds = page.entry(46, "Home-address privacy requirement", address_privacy, when="2026-11-16 UTC") + rounds
    virtual_mail = decide.recap(
        "Yes, legitimate services exist. Your mail goes to a real mail center. Staff show envelope scans online. "
        "You choose opening and scanning, forwarding, storage or shredding. Signup requires identity checks. "
        "That makes it usable for mail, but Google Play acceptance remains unverified. Nothing rented.",
        [
            ("What it is", "A real mailbox at a mail center, managed through an app or website."),
            ("Where mail goes", "Letters arrive at the provider's physical facility."),
            ("Your choices", "View envelopes, request content scans, forward originals, store or shred. Services and charges vary."),
            ("Legitimate use", "USPS recognizes commercial mail receivers. Customers provide identity documents and postal authorization."),
            ("Game publishing", "A usable mailing address is not automatically accepted for Google Play verification."),
        ],
        {
            "did": "Checked postal guidance and providers' explanations of virtual mailbox services.",
            "why": "You asked whether these services are legitimate, usable and where the mail goes.",
            "working": "Physical mail arrives at a real facility. Online controls let customers request handling services.",
            "not_working": "No provider selected or Google acceptance verified. A mailbox does not establish residence or universal legal-address eligibility.",
            "next": "Check a specific address against the intended account requirements before any purchase.",
        },
    )
    virtual_body = "<p>A virtual mailbox combines a real mailing address with an online inbox for managing physical mail.</p>"
    virtual_body += table(["Stage", "What happens"], [
        ("Delivery", "The postal carrier delivers letters to the provider's physical mail facility."),
        ("Preview", "Staff scan the envelope so you can view it online."),
        ("Instructions", "You can request opening and content scans, forwarding, storage or shredding, depending on the service."),
        ("Originals", "Forwarding sends the physical mail to your chosen address. Postage and service fees may apply."),
    ])
    virtual_body += '<p><a href="https://travelingmailbox.com/faqs">Traveling Mailbox explains envelope scans, opening on request and forwarding.</a></p>'
    virtual_body += '<p><a href="https://www.anytimemailbox.com/faqs">Anytime Mailbox describes physical facilities, storage and pickup options.</a></p>'
    virtual_body += '<p><a href="https://faq.usps.com/s/article/Commercial-Mail-Receiving-Agency-CMRA">USPS recognizes commercial mail receivers and requires customer identification and postal authorization.</a></p>'
    virtual_body += "<p>Legitimate services exist for personal and business mail. The provider knows your identity; this is not anonymous mail.</p>"
    virtual_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/15633622?hl=en">Google separately verifies identity and address documents.</a> Its acceptance of a specific mailbox remains unconfirmed.</p>'
    virtual_body += "<p>USPS evidence came from its official search passage; direct page loading failed. Provider descriptions are not location audits.</p>"
    virtual_body += "<p>No provider selected or rented. Your home-address privacy requirement stays in force.</p>"
    virtual_mail += decide.info_thread("aquila-direction-r47-info",
        "Yes, legitimate virtual mailbox services exist. Physical mail goes to their mail center, and you manage it online.",
        body_html=virtual_body, short="How virtual mailboxes work", follows="aquila-direction-r46-info")
    virtual_mail += decide.whats_next("aquila-direction-r47-next",
        "Confirm a specific address meets Google's verification requirements before renting it. No provider or publishing account selected.",
        short="Verify address eligibility before purchase", follows="aquila-direction-r46-next")
    rounds = page.entry(47, "Virtual mailboxes explained", virtual_mail, when="2026-11-16 UTC") + rounds
    store_addresses = decide.recap(
        "Google and Apple have public-address rules. Steam collects an address during onboarding; no equivalent public-listing requirement was found. "
        "Apple permits verified PO boxes for individual EU trader details. Google mailbox acceptance remains unresolved. "
        "Google expands trader contact display worldwide on March 1, 2027.",
        [
            ("Google now", "Paid apps and in-app purchases require public legal addresses. Organization accounts also expose their legal address."),
            ("Google update", "Individual traders' address and phone become visible worldwide on March 1, 2027."),
            ("Apple", "EU trader listings show contact details. Individuals can provide a verified PO box for that display."),
            ("Steam", "Partner onboarding collects an address. No equivalent mandatory public developer-address listing found in reviewed documentation."),
            ("Mailbox", "Google requires matching address proof. No blanket acceptance of virtual mailboxes established."),
        ],
        {
            "did": "Compared official Google, Apple and Steam address rules, including Google's announced trader changes.",
            "why": "You asked which stores require an address and whether Steam differs.",
            "working": "Apple explicitly permits a verified PO box for individual EU trader display, separate from enrollment.",
            "not_working": "Google mailbox acceptance remains unresolved. Steam's reviewed documents do not establish a universal privacy guarantee.",
            "next": "Resolve Google's acceptable separate address before account setup. No purchase, signup or support message authorized.",
        },
    )
    store_address_body = table(["Store", "Public display", "Verification"], [
        ("Google Play", '<a href="https://support.google.com/googleplay/android-developer/answer/13634081?hl=en">Paid apps and in-app purchases require full legal-address display.</a> '
         '<a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Organizations also show their legal address.</a>',
         '<a href="https://support.google.com/googleplay/android-developer/answer/15633622?hl=en">The linked payments profile and accepted proof must match.</a> No blanket mailbox approval found.'),
        ("Apple", '<a href="https://developer.apple.com/help/app-store-connect/manage-compliance-information/manage-european-union-digital-services-act-trader-requirements/">EU trader listings show address, phone and email. Individuals may use a documented PO box.</a>',
         '<a href="https://developer.apple.com/programs/enroll/">Individual enrollment separately requires an address and rejects PO boxes.</a> Public trader contact details are separate.'),
        ("Steam", "No equivalent mandatory public developer-address listing found in the reviewed official documentation.",
         '<a href="https://partner.steamgames.com/doc/gettingstarted/faq">Valve still collects address, legal identity and tax information during partner onboarding.</a>'),
    ])
    store_address_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/17594752?hl=en">Google announces worldwide contact display for traders starting March 1, 2027, including individual address and phone.</a></p>'
    store_address_body += "<p>New Google accounts declare trader status immediately. Existing accounts must declare by February 1, 2027.</p>"
    store_address_body += "<p>Trader status can cover ad-funded or other commercial apps, not only paid downloads. Free pricing alone does not settle status.</p>"
    store_address_body += "<p>These checks distinguish public display from private verification. Steam's documentation silence is not a universal privacy guarantee.</p>"
    store_address_body += "<p>Google's account-details search passage was readable; direct loading timed out. Other cited policy pages were directly readable.</p>"
    store_addresses += decide.info_thread("aquila-direction-r48-info",
        "Google and Apple have explicit public-address rules. Steam still collects an address for its partner account.",
        body_html=store_address_body, short="Public addresses across three stores", follows="aquila-direction-r47-info")
    store_addresses += decide.whats_next("aquila-direction-r48-next",
        "Google's separate-address eligibility remains open before signup. Your home address must stay off public listings; no service selected.",
        short="Keep home address private", follows="aquila-direction-r47-next")
    rounds = page.entry(48, "Store address requirements checked", store_addresses, when="2026-11-16 UTC") + rounds
    developer_guides = decide.recap(
        "Yes. Developers have documented this. Jake Lee describes successful Google verification using a company and forwarding address. "
        "A newer Japanese guide reports success as a sole proprietor using a virtual office. "
        "We have practical examples to adapt to Texas. No business structure or address service selected.",
        [
            ("Best starting guide", "Jake Lee documents failed personal verification, then successful company verification with a forwarding address."),
            ("Recent example", "A Japanese sole proprietor reports verified business accounts using a virtual office and supporting records."),
            ("US reference", "Formation Lane explains LLC setup for Apple and Google. It contains affiliate links."),
            ("Practical pattern", "Use a legitimate business address with matching business records and the appropriate account type."),
            ("Your decision", "An LLC remains optional. These examples do not approve a particular Texas address or provider."),
        ],
        {
            "did": "Read practical developer walkthroughs, including successful verification stories and the failures preceding them.",
            "why": "You asked for guides from people who had already navigated this process.",
            "working": "Published first-hand accounts describe keeping home addresses off listings through verified business addresses.",
            "not_working": "UK and Japanese paperwork cannot be copied directly into a Texas application. No provider tested for your account.",
            "next": "Use these guides to shape a Texas-specific publishing setup. No company formation or address purchase chosen.",
        },
    )
    guide_body = '<p><a href="https://blog.jakelee.co.uk/publishing-on-google-play-without-exposing-info/">Jake Lee: publishing on Google Play without exposing personal details</a> is the strongest starting point.</p>'
    guide_body += "<p>He develops Android games and describes the process he completed, including failed verification attempts.</p>"
    guide_body += table(["Guide", "Documented experience", "Scope"], [
        ('<a href="https://blog.jakelee.co.uk/publishing-on-google-play-without-exposing-info/">Jake Lee, January 2025</a>',
         "Company, forwarding address, business identity number and Google organization account. He reports successful verification and game transfers.",
         "United Kingdom. His older account-transfer procedure is not the only route today."),
        ('<a href="https://zenn.dev/pokecloudlab/articles/google-play-console-personal-to-organization?locale=en">Sole-proprietor walkthrough, August 2026</a>',
         "Virtual office, business records and contract evidence. He reports successful verification after adding missing documents.",
         "Japan. English translation and original reviewed. Account verified; app still unpublished in this article."),
        ('<a href="https://formationlane.com/guides/put-your-app-under-your-llc/">US LLC publishing guide, October 2026</a>',
         "Explains business identity, contact details and Apple/Google organization enrollment.",
         "Commercial guide with affiliate links, not an independent success report or a reason to require an LLC."),
    ])
    guide_body += "<p>My earlier answers stopped too soon at policy uncertainty. These examples provide a practical route to investigate.</p>"
    guide_body += "<p>The pattern: establish a legitimate business address, document it consistently, and verify the corresponding business account.</p>"
    guide_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/16260648?hl=en">Google now supports converting eligible personal accounts to organization accounts.</a> Do not copy outdated migration steps automatically.</p>'
    guide_body += "<p>These are reported outcomes, not verification of your future application. An LLC remains a choice, not a universal publishing requirement.</p>"
    developer_guides += decide.info_thread("aquila-direction-r49-info",
        "Yes. I found detailed developer guides showing how they completed verification while keeping home addresses off store listings.",
        body_html=guide_body, short="Practical developer guides found", follows="aquila-direction-r48-info")
    developer_guides += decide.whats_next("aquila-direction-r49-next",
        "Adapt the documented business-address route to Texas before choosing a service or account. Home-address privacy remains required.",
        short="Use guides for a Texas plan", follows="aquila-direction-r48-next")
    rounds = page.entry(49, "Developer address guides found", developer_guides, when="2026-11-16 UTC") + rounds
    texas_research = decide.recap(
        "Your Texas research prompt is saved and ready for Perplexity. It covers private home addresses without forming a company, "
        "all three stores, and running a profitable side business while employed. "
        "The console stores prompts and results; it does not run Perplexity. No research result has returned.",
        [
            ("Individual route first", "Compare Google Play, Apple and Steam without assuming an LLC is needed."),
            ("Home privacy", "Separate public store details from private verification and public business records."),
            ("Employment", "Check side-business profits, employer restrictions and ownership of your game."),
            ("Practical evidence", "Request Texas rules, firsthand guides, address options, costs and verification documents."),
            ("Research status", "Prompt saved in Research Console. Perplexity must still be run manually."),
        ],
        {
            "did": "Saved one complete research prompt with all three additions and a tracked request.",
            "why": "You want a workable Texas publishing route without exposing your home address.",
            "working": "Research Console accepted the prompt under Aquila. Its receipt records the exact content hash.",
            "not_working": "No Perplexity result exists yet. No company, address service or publishing account selected.",
            "next": "Run the prompt in Perplexity and save its complete answer in the matching request.",
        },
    )
    research_url = "http://127.0.0.1:8776/?project=aquila-code-dark#bd6cbf0bf4e64935a6b993aa171316df"
    research_body = table(["Research section", "Required answer"], [
        ("Individual accounts", "Can a Texas resident publish privately on each store without forming a company?"),
        ("Address evidence", "Which addresses and documents work for public contact, private verification and business records?"),
        ("Texas alternatives", "Compare personal publishing, assumed names and optional LLC formation, including costs and public records."),
        ("Employment", "Can an employee own and profit from an LLC? Check contract restrictions, conflicts and game ownership."),
        ("Developer experience", "Prioritize firsthand Texas or US guides. Label overseas examples, vendor claims and unproven assumptions."),
    ])
    research_body += '<p>The prompt includes current and announced rules, public names, mailbox options, tax basics and specific unresolved questions.</p>'
    research_body += '<p>Shared knowledge searches found no usable Texas publishing or moonlighting guidance in the inspected results.</p>'
    research_body += '<p>The local console accepted the request. This proves prompt storage, not completed research or platform acceptance.</p>'
    texas_research += decide.info_thread("aquila-direction-r50-info",
        "Added individual publishing privacy and employment to the Texas research request. All three stores are included.",
        body_html=research_body, short="Texas research prompt ready", follows="aquila-direction-r49-info")
    run_steps = f'<p><a href="{research_url}">Open the Aquila Texas research request</a>.</p>'
    run_steps += table(["Step", "Action"], [
        ("1", "Copy the complete prompt into Perplexity and run its research mode."),
        ("2", "Paste or import the complete result into this same request."),
        ("3", "Press Save result, then tell me the result is ready."),
    ])
    texas_research += decide.request("aquila-direction-r50-run",
        "Please run the saved prompt in Perplexity and save its complete result in Research Console.",
        summary="This console tracks research but cannot run Perplexity itself.",
        info=run_steps, short="Run and save Texas research", reply="Has the complete result been saved?")
    texas_research += decide.whats_next("aquila-direction-r50-next",
        "Review the saved research and compare practical routes before choosing an address service or business structure.",
        summary="Waiting for the Perplexity result. Home-address privacy remains required.",
        short="Review research before choosing", follows="aquila-direction-r49-next")
    rounds = page.entry(50, "Texas research ready for Perplexity", texas_research, when="2026-11-16 UTC") + rounds
    research_review = decide.recap(
        "Read the full research and checked its key sources. You do not need an LLC solely to publish. "
        "Apple permits a documented public PO box; Steam accepts individuals. Google's mailbox route remains unproven. "
        "Employment terms and Texas public records still matter. No setup choice made.",
        [
            ("Company", "An LLC is optional. It does not automatically solve address privacy."),
            ("Steam", "Individual publishing is explicit. No equivalent public street-address requirement found in the reviewed documentation."),
            ("Apple", "Individuals may use a documented PO box for public EU trader contact details."),
            ("Google", "Paid apps and purchases expose the payments address. A reliable Texas mailbox route remains unresolved."),
            ("Employment", "A side business is generally possible. Employer restrictions and ownership of the game depend on actual terms."),
        ],
        {
            "did": "Read the complete returned research, preserved its original and checked the main platform and Texas sources.",
            "why": "You asked to review the saved research before choosing a publishing setup.",
            "working": "Official sources support individual publishing and Apple's separate public PO-box option.",
            "not_working": "Google mailbox eligibility, specific providers and your employment terms remain unverified.",
            "next": "Resolve a specific Google address arrangement before buying a service or opening publishing accounts.",
        },
    )
    review_body = table(["Store", "Supported route", "Limit"], [
        ("Steam", '<a href="https://partner.steamgames.com/doc/gettingstarted/onboarding">Publish as an individual sole proprietor.</a> '
         '<a href="https://partner.steamgames.com/doc/store/creator_homepage">Public developer and publisher labels are separate.</a>',
         "No equivalent public street-address requirement found in reviewed pages. This is not a worldwide secrecy guarantee."),
        ("Apple", '<a href="https://developer.apple.com/help/app-store-connect/manage-compliance-information/manage-european-union-digital-services-act-trader-requirements/">A documented PO box can replace your home address in individual EU trader contact details.</a>',
         '<a href="https://developer.apple.com/programs/enroll/">Enrollment needs separate address verification. Your personal seller name remains public.</a>'),
        ("Google Play", '<a href="https://support.google.com/googleplay/android-developer/answer/13634885?hl=en">Personal accounts can earn money.</a> '
         '<a href="https://support.google.com/googleplay/android-developer/answer/13634081?hl=en">Paid apps and in-app purchases require public address display.</a>',
         '<a href="https://support.google.com/googleplay/android-developer/answer/15633622?hl=en&amp;co=GENIE.CountryCode=US">Accepted proof documents do not establish blanket mailbox eligibility.</a> No proven Texas personal-account route found.'),
    ])
    review_body += '<p><a href="https://efte.twc.texas.gov/conflict_of_interest.html">Texas guidance recognizes outside jobs and business interests, alongside employer conflict restrictions.</a></p>'
    review_body += '<p>LLC ownership and profits are generally compatible with employment. Permission requirements depend on actual contracts and policies.</p>'
    review_body += '<p><a href="https://www.copyright.gov/circs/circ30.pdf">Employee work within employment scope can belong to the employer.</a> Personal time alone does not settle every ownership claim.</p>'
    review_body += '<p><a href="https://tcss.legis.texas.gov/resources/BC/htm/BC.71.htm">Texas individual DBA paperwork requires your residence address.</a> It is not a home-address privacy shortcut.</p>'
    review_body += '<p><a href="https://comptroller.texas.gov/taxes/sales/marketplace-providers-sellers.php">Texas marketplace sellers can still have permit and return duties.</a> Your exact store agreements and revenue model need classification.</p>'
    review_body += '<p>My recommendation: do not form an LLC solely because you think publishing requires it or automatically protects your address.</p>'
    review_body += '<p>The original compares address vendors, but none has proven acceptance for your account. No provider selected.</p>'
    review_body += f'<p><a href="{research_url}">Complete research and saved version</a>. Version 1 was preserved unchanged; core claims were checked, not every citation or price.</p>'
    review_body += '<p>The research request is complete. Existing release targets remain; no company, account, purchase or support contact started.</p>'
    research_review += decide.info_thread("aquila-direction-r51-info",
        "Research reviewed. An LLC is optional; Google's acceptable non-home address remains the main publishing privacy gap.",
        body_html=review_body, short="Research reviewed and checked", follows="aquila-direction-r50-info")
    research_review += decide.whats_next("aquila-direction-r51-next",
        "Check a concrete Google address arrangement and its proof before selecting a business structure or buying an address service.",
        summary="No decision or action needed from you for this review. Account setup remains undecided.",
        short="Resolve the specific address route", follows="aquila-direction-r50-next")
    rounds = page.entry(51, "Texas research reviewed", research_review, when="2026-11-16 UTC") + rounds
    google_focus = decide.recap(
        "Yes, your concern is clear. You want a personal Google Play account without publishing your home address. "
        "Google allows personal accounts to earn money. Paid apps and purchases require a public address. "
        "We still need an accepted non-home address. Apple also has public EU address rules, but allows a PO box.",
        [
            ("Your requirement", "Your home address must not appear publicly on the game listing."),
            ("Account", "A personal Google Play account can earn money."),
            ("Public address", "Paid apps and in-app purchases require the linked payments address to be public."),
            ("Open point", "Which legitimate non-home address will Google accept for that personal account?"),
            ("Small correction", "Apple also requires public EU trader contact details, but explicitly allows individuals to use a PO box."),
        ],
        {
            "did": "Narrowed the discussion to your personal Google account and public home-address concern.",
            "why": "You want the address issue answered without expanding into company formation or unrelated setup.",
            "working": "Google's rules explicitly allow personal accounts to monetize.",
            "not_working": "No acceptable non-home address has been verified for your personal Google account.",
            "next": "Keep the next investigation focused on that address and its required proof.",
        },
    )
    focus_body = '<p><a href="https://support.google.com/googleplay/android-developer/answer/13634885?hl=en">Google permits personal accounts to earn money.</a> That part is established.</p>'
    focus_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/13634081?hl=en">Paid apps and in-app purchases require public display of the linked payments-profile address.</a></p>'
    focus_body += '<p>The remaining issue is which legitimate alternative to your home address Google will accept for that personal account.</p>'
    focus_body += '<p><a href="https://developer.apple.com/help/app-store-connect/manage-compliance-information/manage-european-union-digital-services-act-trader-requirements/">Apple also requires a public EU trader address, but explicitly permits individuals to use a documented PO box.</a></p>'
    google_focus += decide.info_thread("aquila-direction-r52-info",
        "Yes. You want a personal Google Play account without making your home address public.",
        body_html=focus_body, short="Focus on Google home-address privacy", follows="aquila-direction-r51-info")
    google_focus += decide.whats_next("aquila-direction-r52-next",
        "Resolve which non-home address Google accepts for a personal account, and what proof it requires.",
        summary="Requirement understood. No action needed from you for this clarification.",
        short="Resolve the Google address", follows="aquila-direction-r51-next")
    rounds = page.entry(52, "Google address concern clarified", google_focus, when="2026-11-16 UTC") + rounds
    apple_region = decide.recap(
        "Yes, EU means European Union. You live in Texas. Apple's rule depends on where you distribute your game. "
        "If you offer it only in the United States, that EU public-address rule does not apply. "
        "If you offer it in the EU, it can apply despite living in Texas. I should have stated that condition.",
        [
            ("Your location", "You are in Texas, United States."),
            ("Rule trigger", "Apple's EU trader disclosure concerns games offered on EU storefronts."),
            ("United States only", "That EU public-address rule does not apply to a game offered only in the United States."),
            ("EU availability", "A Texas developer offering a commercial game in the EU falls under that distribution rule."),
            ("Correction", "I should have made the condition clear. Your release countries have not been selected."),
        ],
        {
            "did": "Clarified that Apple's EU disclosure depends on distribution territory, not your residence.",
            "why": "My earlier wording made the rule sound unconditional for you in Texas.",
            "working": "Apple explicitly ties public contact display to apps distributed in EU territories.",
            "not_working": "No release-country selection exists yet. This does not settle Google's separate address rules.",
            "next": "Keep Google's public-address issue as the focus. Treat Apple's EU condition only when relevant to chosen markets.",
        },
    )
    region_body = '<p>EU means European Union. This concerns where customers can get your game, not where you live.</p>'
    region_body += '<p><a href="https://developer.apple.com/help/app-store-connect/manage-compliance-information/manage-european-union-digital-services-act-trader-requirements/">Apple ties this public contact display to distribution in EU territories.</a></p>'
    region_body += '<p>A United States-only release avoids that specific EU display requirement. Private account verification remains separate.</p>'
    region_body += '<p>No launch-country decision has been made. I should not have presented the EU condition as automatic.</p>'
    apple_region += decide.info_thread("aquila-direction-r53-info",
        "You are in Texas. Apple's EU rule depends on distributing your game there; I should have stated that condition.",
        body_html=region_body, short="EU distribution, not residence", follows="aquila-direction-r52-info")
    apple_region += decide.whats_next("aquila-direction-r53-next",
        "Keep the discussion focused on Google's public-address requirement. Choose release countries separately when needed.",
        summary="No action or decision needed from you for this clarification.",
        short="Keep the Google privacy focus", follows="aquila-direction-r52-next")
    rounds = page.entry(53, "Apple's EU condition explained", apple_region, when="2026-11-16 UTC") + rounds
    us_scope = decide.recap(
        "Yes. We can plan for United States sales only. Apple lets you choose countries. "
        "EU sales need verified trader details, not an address in Europe. "
        "That EU public-address rule stays outside our current plan. Google's paid-app address requirement still needs resolving.",
        [
            ("Current scope", "Use United States sales only as our planning assumption for now."),
            ("Country selection", "Apple lets you select specific countries before submitting your game."),
            ("EU address", "The contact address for EU distribution does not have to be in Europe."),
            ("EU availability", "A commercial release there requires completed trader verification."),
            ("Google remains", "United States-only sales do not remove Google's public-address requirement for paid apps."),
        ],
        {
            "did": "Saved United States-only sales as the current planning assumption and checked Apple's country selection.",
            "why": "You want to keep this discussion focused on selling in the United States.",
            "working": "Apple supports selecting specific countries. The EU public-contact rule applies to EU distribution.",
            "not_working": "Google's acceptance of a non-home address for your personal account remains unresolved.",
            "next": "Keep the address discussion within United States sales. No decision or action needed from you now.",
        },
    )
    us_body = '<p><a href="https://developer.apple.com/help/app-store-connect/manage-your-apps-availability/manage-availability-for-your-app-on-the-app-store/">Apple lets you choose specific countries before submission.</a> We can select the United States only.</p>'
    us_body += '<p><a href="https://developer.apple.com/help/app-store-connect/manage-compliance-information/manage-european-union-digital-services-act-trader-requirements/">EU distribution requires verified trader contact details.</a> The address does not have to be in Europe.</p>'
    us_body += '<p><a href="https://developer.apple.com/news/upcoming-requirements/?id=02172025a">Apple withholds EU availability until required trader verification is complete.</a></p>'
    us_body += '<p>United States-only sales avoid that specific Apple EU public-contact requirement.</p>'
    us_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/13634081?hl=en">Google still requires a public address for paid apps.</a> An accepted non-home address remains unresolved.</p>'
    us_scope += decide.info_thread("aquila-direction-r54-info",
        "United States only is our planning assumption for now. No store settings changed.",
        body_html=us_body, short="Plan for United States sales", follows="aquila-direction-r53-info")
    us_scope += decide.whats_next("aquila-direction-r54-next",
        "Keep the discussion focused on Google's address requirement for United States sales.",
        summary="No action needed from you now. Broader markets can be considered later.",
        short="Keep the current scope", follows="aquila-direction-r53-next")
    rounds = page.entry(54, "United States sales for now", us_scope, when="2026-11-16 UTC") + rounds
    address_options = decide.recap(
        "Both have address services worth comparing. Northwest lists $99/year with a lease and 20 mail scans. "
        "Texan lists $19/month with a lease and unlimited scans. "
        "Neither establishes Google acceptance for your personal account. An LLC remains optional.",
        [
            ("Relevant service", "Compare their virtual-office plans for your public-address goal."),
            ("Northwest", "$99/year includes a Texas address, suite number, lease and 20 mail scans."),
            ("Texan", "$19/month includes an Austin address, suite number, lease, phone and unlimited scans."),
            ("Google proof", "An office lease alone is not listed among accepted address documents for United States personal accounts."),
            ("Recommendation", "Confirm Google's acceptance of the address and proof before paying for either plan."),
        ],
        {
            "did": "Compared both providers' published address services and Google's United States verification requirements.",
            "why": "You supplied two possible services for keeping your home address private.",
            "working": "Both offer commercial addresses and mail handling. Northwest explicitly offers its virtual office separately.",
            "not_working": "Neither provider's reviewed pages establish Google acceptance for your personal account.",
            "next": "Resolve Google address eligibility before purchase. No provider selected or contacted.",
        },
    )
    address_body = '<p>Your links promote registered-agent and LLC services. Their virtual-office plans better match the address question.</p>'
    address_body += table(["Provider", "Published plan", "Mail and documents"], [
        ('<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Northwest, Texas virtual office</a>', '$49/year', 'Texas street address, unique suite number and 15 scans. No office lease included.'),
        ('<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Northwest, lease option</a>', '$99/year', 'Texas street address, unique suite number, 20 scans and office lease.'),
        ('<a href="https://www.texasregisteredagent.net/texas-business-address/texas-virtual-office/">Texan Registered Agent, virtual office</a>', '$19/month; $228 over 12 months', 'Austin address, unique suite number, unlimited scans, month-to-month lease and phone line.'),
    ])
    address_body += '<p>These are advertised service prices. Northwest explicitly offers its virtual office separately from LLC formation and registered-agent service.</p>'
    address_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/15633622?hl=en&amp;co=GENIE.CountryCode%3DUS">Google requires matching personal-account address proof.</a> An office lease is not on its United States list.</p>'
    address_body += '<p>Bank statements are listed, but that alone does not establish acceptance of a virtual address.</p>'
    address_body += '<p>I would compare these address services before choosing an LLC package. Neither option is verified for your Google account.</p>'
    address_body += '<p>The Texas advertising page did not load through the research tool. Its main address-service pages supplied the comparison.</p>'
    address_options += decide.info_thread("aquila-direction-r55-info",
        "Both offer relevant address services. Google acceptance remains unverified.",
        body_html=address_body, short="Compare the address services", follows="aquila-direction-r54-info")
    address_options += decide.whats_next("aquila-direction-r55-next",
        "Before purchase, establish whether Google accepts the chosen address and available documents for your personal account.",
        summary="United States-only planning continues. No decision or action needed from you now.",
        short="Resolve acceptance before purchase", follows="aquila-direction-r54-next")
    rounds = page.entry(55, "Two address services compared", address_options, when="2026-11-16 UTC") + rounds
    focused_research = decide.recap(
        "Focused Perplexity prompt is ready for Northwest and Texan. "
        "It asks whether a United States personal Google Play account can use their addresses, and which proof works. "
        "It also seeks documented approvals and rejections. Run the prompt in Perplexity, then save the full answer in Research Console.",
        [
            ("Two providers", "Northwest and Texan Registered Agent are the exact candidates."),
            ("Your account", "Research prioritizes a personal account, without assuming an LLC."),
            ("Address proof", "Check allowed address types, accepted documents and public versus private fields."),
            ("Real outcomes", "Find dated approvals and rejections, identifying country, account type, provider and documents."),
            ("Ready to run", "The prompt is stored. Research Console cannot run Perplexity itself."),
        ],
        {
            "did": "Prepared and stored the focused research prompt, with an exact readback check.",
            "why": "You want evidence that Google accepts these addresses before choosing a service.",
            "working": "The saved request covers both providers and the personal-account scenario.",
            "not_working": "Perplexity has not run through this workflow. No new results or Google approval exist.",
            "next": "Run the prompt and save the full report. I will review the returned evidence.",
        },
    )
    focused_url = "http://127.0.0.1:8776/?project=aquila-code-dark#cf71e97b7df84637b5180b6ea8662f0d"
    focused_body = table(["Research question", "Evidence requested"], [
        ("Personal eligibility", "Can an individual use either exact address service without forming a company?"),
        ("Google address fields", "Separate private identity checks, linked payments address and public store display."),
        ("Supported documents", "Check address eligibility separately from whether a lease or bank statement is an accepted document."),
        ("Successful and failed cases", "Require dates, countries, account types, providers, documents and outcomes."),
        ("Clear verdict", "Supported with conditions, explicitly disallowed, or not established for each provider."),
    ])
    focused_body += f'<p><a href="{focused_url}">Open the complete focused research prompt</a>.</p>'
    focused_body += '<p>United States sales remain the scope. Organization accounts appear only as a brief fallback comparison.</p>'
    focused_body += '<p>The earlier broad Texas report remains preserved. This request targets its unresolved address question.</p>'
    focused_body += '<p>Prompt storage and exact readback passed. No research result has returned.</p>'
    focused_research += decide.info_thread("aquila-direction-r56-info",
        "Focused Perplexity research is ready for both address providers.",
        body_html=focused_body, short="Focused address research ready", follows="aquila-direction-r55-info")
    focused_steps = f'<p><a href="{focused_url}">Open this research request</a>.</p>'
    focused_steps += table(["Step", "Action"], [
        ("1", "Copy the complete prompt into Perplexity and run its research mode."),
        ("2", "Paste or import the complete report into this same request."),
        ("3", "Press Save result, then tell me the report is ready."),
    ])
    focused_research += decide.request("aquila-direction-r56-run",
        "Please run this focused prompt in Perplexity, then save the complete result in its Research Console request.",
        summary="Research Console stores prompts and results. It cannot execute Perplexity.",
        info=focused_steps, short="Run the focused research", reply="Have you saved the complete report in this request?")
    focused_research += decide.whats_next("aquila-direction-r56-next",
        "Review the returned evidence and determine whether either provider supports your personal-account privacy goal.",
        summary="Waiting for the Perplexity report. No address purchase or account setup started.",
        short="Review the returned evidence", follows="aquila-direction-r55-next")
    rounds = page.entry(56, "Focused Google address research", focused_research, when="2026-11-16 UTC") + rounds
    address_review = decide.recap(
        "A virtual address remains a plausible route without forming an LLC. Texan explicitly accepts individuals. "
        "A Google community expert supports virtual mailboxes with matching proof, but that is not official approval. "
        "Neither provider has a confirmed approval for your account type. Ask Google about the exact service before buying.",
        [
            ("Research read", "The complete report is preserved and its exact version marked reviewed."),
            ("Texan", "Its published mail-service terms explicitly accept individuals without a company."),
            ("Google community", "A volunteer Product Expert supports virtual mailboxes with matching supporting documents."),
            ("Remaining gap", "Neither candidate has a verified approval for this United States personal-account case."),
            ("Next step", "Get Google's answer about the exact service, proof and public address before paying."),
        ],
        {
            "did": "Read the returned research, checked key sources, preserved the original and recorded review.",
            "why": "You want to sell through a personal Google account without exposing your home address.",
            "working": "Texan serves individuals. Conditional community guidance supports documented virtual mailboxes.",
            "not_working": "No confirmed Google approval exists for either provider in your exact account scenario.",
            "next": "Clarify the specific address and proof with Google before purchase. No message has been sent.",
        },
    )
    review_body = table(["Finding", "Evidence", "Limit"], [
        ("Texan accepts individuals",
         '<a href="https://www.texasregisteredagent.net/texas-mail-forwarding/">Its mail-service page allows customers without a business.</a> Virtual office costs $19/month.',
         "Provider eligibility does not establish Google acceptance."),
        ("Northwest offers standalone service",
         '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Texas plans include an address and suite.</a> A lease and 20 scans cost $99/year.',
         "No completed approval found for this provider and personal-account scenario."),
        ("Google community gives conditional support",
         '<a href="https://support.google.com/googleplay/android-developer/thread/372736662/is-a-virtual-private-mailbox-an-acceptable-address-for-personal-or-organization-accounts?hl=en">BenMcc supports virtual mailboxes with matching documents, such as bank statements.</a>',
         "Volunteer Product Expert, not a Google employee. Different providers; no completed verification shown."),
        ("Official requirements still apply",
         '<a href="https://support.google.com/googleplay/android-developer/answer/7161426?hl=en">Google requires a valid physical address matching official documents and excludes PO boxes.</a>',
         "Reviewed rules neither approve these providers nor establish a blanket prohibition on every virtual office."),
    ])
    review_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/15633622?hl=en&amp;co=GENIE.CountryCode%3DUS">Matching bank statements are listed United States address proofs.</a> An office lease is not listed there.</p>'
    review_body += '<p>An accepted document type does not guarantee acceptance of every address printed on it.</p>'
    review_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/16260648?hl=en">Identity and merchant profiles can differ.</a> <a href="https://support.google.com/googleplay/android-developer/answer/7162811?hl=en">The public merchant address is editable.</a></p>'
    review_body += '<p>Those options do not establish that your home stays hidden across every public developer and merchant field.</p>'
    review_body += '<p>Texan has clearer published individual eligibility. That makes it worth investigating, without assuming an LLC is needed.</p>'
    review_body += f'<p><a href="{focused_url}">Full returned research and recorded review</a>. Original preserved unchanged; key sources checked, not every anecdote.</p>'
    review_body += '<p>The run/save request is complete. No provider selected, purchase made or support message sent.</p>'
    address_review += decide.info_thread("aquila-direction-r57-info",
        "Virtual addresses remain plausible. Neither provider has confirmed Google approval for your personal account.",
        body_html=review_body, short="Evidence and remaining uncertainty", follows="aquila-direction-r56-info")
    address_review += decide.whats_next("aquila-direction-r57-next",
        "Seek Google's written answer about the exact address, supporting documents and public fields before paying.",
        summary="No action needed from you for this review. An unsent support draft is saved; Google may not preapprove hypothetical addresses.",
        short="Clarify the exact address", follows="aquila-direction-r56-next")
    rounds = page.entry(57, "Google address research reviewed", address_review, when="2026-11-16 UTC") + rounds
    google_contact = decide.recap(
        "Google has a contact form for problems creating or accessing an account. People have asked about virtual addresses. "
        "One developer posted support replies refusing them for personal accounts. Those conflict with volunteer guidance. "
        "We need clarification for your United States account, not another broad research report.",
        [
            ("Contact route", "Google's fallback form includes account creation and Individual options."),
            ("Before enrollment", "Explain that address eligibility remains unresolved, preventing you from proceeding with account creation."),
            ("Others asked", "A developer posted replies attributed to Google support refusing personal-account virtual offices."),
            ("Evidence limit", "Those copied emails are not independently authenticated or established as applicable to your Texas account."),
            ("Revised assessment", "The conflicting replies reduce confidence in the personal-account workaround. No message has been sent."),
        ],
        {
            "did": "Found Google's account-creation contact form and posted replies attributed to its support team.",
            "why": "You asked how to contact Google and whether earlier developers already published answers.",
            "working": "A public form exists. Previous questions and conflicting answers are documented.",
            "not_working": "The personal-account virtual-address route still lacks a reliable answer for your United States case.",
            "next": "Use the prepared wording for focused support clarification if you choose to contact Google.",
        },
    )
    contact_body = '<p><a href="https://support.google.com/googleplay/android-developer/contact/general_contact">Google Play developer contact form</a> covers people unable to create or access Play Console.</p>'
    contact_body += table(["Step", "Action"], [
        ("1", 'Choose "Creating a developer account" and "Individual".'),
        ("2", "Provide your contact email and United States location."),
        ("3", "Explain that you need address eligibility clarified before proceeding with enrollment."),
    ])
    contact_body += '<p>The public form does not mark developer ID as required. Actual submission and support response remain untested.</p>'
    contact_body += '<p>For existing accounts, <a href="https://support.google.com/googleplay/android-developer/gethelp">Google routes support through Play Console Help</a>.</p>'
    contact_body += table(["Posted answer", "What it establishes"], [
        ('<a href="https://www.reddit.com/r/googleplayconsole/comments/1vuapmd/hur_har_ni_l%C3%B6st_detta_legal_address_requirements/">August 2026 support exchange</a>',
         "A Swedish-language thread contains copied replies requiring residential addresses for personal accounts and refusing virtual offices. Attribution remains unverified."),
        ('<a href="https://support.google.com/googleplay/android-developer/thread/372736662/is-a-virtual-private-mailbox-an-acceptable-address-for-personal-or-organization-accounts?hl=en">September 2025 community answer</a>',
         "A volunteer Product Expert says virtual mailboxes can work with matching documents. This conflicts with the posted support replies."),
    ])
    contact_body += '<p>The missing piece is a reliable answer for your case. It is not an absence of previous questions.</p>'
    contact_body += '<p>This new evidence makes me less confident in the personal-account workaround. It does not establish a Texas-specific prohibition.</p>'
    contact_body += '<p>Prepared wording, not sent:</p>'
    contact_body += '<p>I live in Texas and plan a personal developer account for a paid game sold only in the United States.</p>'
    contact_body += '<p>Before enrolling, I need to confirm which address I may provide.</p>'
    contact_body += '<p>May I use a commercial virtual-office address from Northwest Registered Agent or Texan Registered Agent?</p>'
    contact_body += '<p>What documents must show my personal legal name and that address?</p>'
    contact_body += '<p>If residential verification is required, can that address stay private while a separate address appears publicly?</p>'
    contact_body += '<p>Please distinguish developer-profile and merchant fields and link the applicable United States policy.</p>'
    google_contact += decide.info_thread("aquila-direction-r58-info",
        "Google has a contact form. Other developers have posted answers, including support replies conflicting with the volunteer guidance.",
        body_html=contact_body, short="Contact route and posted answers", follows="aquila-direction-r57-info")
    google_contact += decide.whats_next("aquila-direction-r58-next",
        "Ask Google's account creation support which address and proof a United States personal account can use.",
        summary="Form and draft are ready. No message, account signup, purchase or company decision made.",
        short="Clarify the conflicting answers", follows="aquila-direction-r57-next")
    rounds = page.entry(58, "Google contact and posted replies", google_contact, when="2026-11-16 UTC") + rounds
    company_address = decide.recap(
        "Yes. A company account verifies the company's legal identity and address. Personal verification concerns yours. "
        "Zensoul LLC names Northwest and displays the same business address on Google Play. This is a real company example. "
        "Its Wyoming setup does not guarantee approval of every Texas address.",
        [
            ("Personal account", "Google verifies your individual legal identity and address."),
            ("Organization account", "Google verifies the company's name and address against its Dun & Bradstreet record."),
            ("Northwest example", "Zensoul LLC names Northwest, and its Google Play listing shows the matching Wyoming address."),
            ("Paid app evidence", "That listing describes an annual membership renewing through Google Play."),
            ("Remaining check", "Confirm the exact Texas business-address service and company documentation. No company or account type selected."),
        ],
        {
            "did": "Compared account requirements and matched a Northwest-linked company's website address to its Google Play listing.",
            "why": "You asked whether an LLC changes the virtual-address situation compared with a personal account.",
            "working": "A monetized LLC listing uses the same address its website attributes to Northwest.",
            "not_working": "Its service contract and verification documents are private. Your Texas setup has not been tested.",
            "next": "Evaluate the exact company-address plan if you choose the organization route. No purchase or filing started.",
        },
    )
    company_body = '<p><a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google documents separate personal and organization verification requirements.</a></p>'
    company_body += table(["Account", "Identity checked", "Address consequence"], [
        ("Personal", "Your individual legal name and address.", "Virtual-office substitution has conflicting guidance and reported refusals."),
        ("Organization", "The company's legal name and address, matching Dun & Bradstreet and Google payments records.", "A documented business address can be used for company identity; the public example below demonstrates this route."),
    ])
    company_body += '<p>Owning an LLC while keeping a personal account does not change which identity Google verifies.</p>'
    company_body += table(["Northwest-linked example", "Observed evidence"], [
        ('<a href="https://zensoul.app/">Zensoul company website</a>', "Names Zensoul LLC and Northwest Registered Agent Service Inc., with a Sheridan, Wyoming business address."),
        ('<a href="https://play.google.com/store/apps/details?gl=US&amp;hl=en-US&amp;id=app.zensoul.app">Zensoul on Google Play</a>', "Shows Zensoul LLC at the matching street, suite, city and state. Describes a paid annual membership through Google Play."),
    ])
    company_body += '<p>Both pages identify <code>30 N Gould St Ste N, Sheridan, WY 82801</code>. Google adds the ZIP extension <code>6317</code>.</p>'
    company_body += '<p>This establishes a public company example. Its private contract, verification documents and exact Northwest plan were not inspected.</p>'
    company_body += '<p><a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Northwest offers a Texas business address, unique suite and optional lease.</a> Virtual office can accompany LLC formation or stand alone.</p>'
    company_body += '<p>Registered-agent service and business-address service are not automatically the same purchased plan. Check permitted use and supporting company documents.</p>'
    company_body += '<p>The Wyoming example does not approve your Texas address. No formation, purchase, support message or account signup started.</p>'
    company_address += decide.info_thread("aquila-direction-r59-info",
        "Yes. Company verification uses company identity; personal verification uses yours. A Northwest-linked LLC example exists on Google Play.",
        body_html=company_body, short="Company route and Northwest example", follows="aquila-direction-r58-info")
    company_address += decide.whats_next("aquila-direction-r59-next",
        "If you pursue the company route, verify the Texas address plan and matching company records before enrollment.",
        summary="No entity or account type selected. Nothing needed from you for this comparison.",
        short="Check the company address plan", follows="aquila-direction-r58-next")
    rounds = page.entry(59, "Company addresses and Northwest", company_address, when="2026-11-16 UTC") + rounds
    business_timing = decide.recap(
        "Business setup can wait while we prove the game. Current PC and phone tests need no LLC or Play Console. "
        "Northwest offers a free domain and email. You can generally own an LLC while employed, "
        "but your contract and game ownership terms need checking first.",
        [
            ("Testing now", "Local Windows and USB phone tests already work without a company or publisher account."),
            ("Setup later", "Complete company records and enrollment before store testing; obtaining a D-U-N-S number can take 30 days."),
            ("Business email", "Northwest currently offers a free basic domain and email without requiring LLC formation."),
            ("Ownership", "You would be the sole LLC member. Northwest's service does not make it a co-owner."),
            ("Existing job", "Check outside-work rules, conflicts and intellectual property terms before relying on independent game ownership."),
        ],
        {
            "did": "Checked current email terms, enrollment timing, LLC ownership and Texas employment guidance.",
            "why": "You want to prove the game before paying for business setup, while keeping your address private.",
            "working": "Local testing needs no publisher account. Business email and sole ownership fit the proposed path.",
            "not_working": "Your employment terms and exact Texas address acceptance remain unverified.",
            "next": "Review employment terms now; plan business enrollment after playable proof and before store-specific testing.",
        },
    )
    timing_body = table(["Stage", "What is needed"], [
        ("Current development", "No LLC or Play Console account needed for our Windows and USB Android tests. Those tests already passed."),
        ("Google login", "An ordinary Google login can precede LLC formation. A Play Console organization account needs the company's legal identity."),
        ("Store tests", "Play-hosted beta tests and end-to-end Play billing tests need publisher setup. Local installation does not prove store approval."),
        ("Recommended timing", "Review employment terms now. Prove a convincing playable slice, then complete company and publisher setup before store testing."),
    ])
    timing_body += '<p><a href="https://developer.android.com/studio/run/device">Android documents direct device testing</a>. No store account was used for Aquila\'s phone checks.</p>'
    timing_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google requires company details and a D-U-N-S record</a>. Obtaining that number can take up to 30 days.</p>'
    timing_body += '<p>Allow formation and verification time before store tests. A launch-week signup would leave avoidable uncertainty.</p>'
    timing_body += table(["Question", "Answer"], [
        ("Does Northwest supply email?", '<a href="https://www.northwestregisteredagent.com/free-identity-services">Yes, its current basic offer includes a free domain and email without an LLC purchase.</a> Usage limits apply.'),
        ("Can it use our studio domain?", '<a href="https://www.northwestregisteredagent.com/email">Yes, custom domain email is supported.</a> You can keep public business contact separate from personal email.'),
        ("Does signup expose my home?", "Giving an email provider your identity does not automatically publish your home address to email recipients."),
        ("What protects domain records?", '<a href="https://www.northwestregisteredagent.com/domains">Northwest includes domain privacy.</a> Email separation does not guarantee anonymity or resolve Google\'s legal-address requirement.'),
        ("Who owns the LLC?", '<a href="https://www.sos.texas.gov/corp/formationfaqs.shtml">You would be its sole member, meaning owner.</a> Northwest provides services, not ownership participation.'),
        ("Can I keep my job?", "Generally, yes. Employment alone does not prevent LLC ownership or profits; employer restrictions can still apply."),
    ])
    timing_body += '<p><a href="https://efte.twc.texas.gov/conflict_of_interest.html">Texas guidance discusses employer restrictions on outside work and business interests</a>. It does not impose universal employer approval.</p>'
    timing_body += '<p>Check outside-work approval, disclosure, competition, confidentiality and intellectual property assignment terms.</p>'
    timing_body += '<p>Use personal time and equipment. Those precautions do not override your contract.</p>'
    timing_body += '<p><a href="https://www.copyright.gov/register/se-hire.html">Work within employment duties can belong to the employer</a>. Have unclear ownership clauses reviewed by a Texas employment lawyer.</p>'
    timing_body += '<p><a href="https://www.irs.gov/businesses/small-businesses-self-employed/single-member-limited-liability-companies">A single-member LLC normally reports activity on its owner\'s federal return</a>. Corporate elections can change that treatment.</p>'
    timing_body += '<p>Your Northwest preference is saved as tentative. No company formed, service purchased, account created or employer contacted.</p>'
    timing_body += '<p>Exact Texas address acceptance and your employment terms remain unverified. Native implementation remains paused for this discussion.</p>'
    business_timing += decide.info_thread("aquila-direction-r60-info",
        "Yes, you can defer setup. Local testing works now, and an LLC can generally coexist with employment.",
        body_html=timing_body, short="Business timing, email and employment", follows="aquila-direction-r59-info")
    business_timing += decide.whats_next("aquila-direction-r60-next",
        "Check employment and ownership terms before commercial plans advance; defer enrollment until the game has convincing playable proof.",
        summary="This is the recommended sequence. No purchase or development resumption started, and nothing is needed for today's explanation.",
        short="Prove the game before enrollment", follows="aquila-direction-r59-next")
    rounds = page.entry(60, "LLC timing and your existing job", business_timing, when="2026-11-16 UTC") + rounds
    business_saved = decide.recap(
        "Saved one reference covering the business discussion, sources, your choices and open questions. "
        "Future sessions are directed to read it first. Both original research reports remain preserved. "
        "Your Northwest preference stays tentative; no setup started.",
        [
            ("Reference", "Saved the LLC, email, employment, studio branding and store privacy findings together."),
            ("Your choices", "Recorded Texas, sole ownership, United States planning, home-address privacy and your tentative Northwest preference."),
            ("Evidence", "Linked sources and both preserved research reports without changing their originals."),
            ("Future sessions", "Updated task routes, knowledge index and restart records to use the reference first."),
            ("Open issues", "Kept employer terms, exact address approval and final branding unresolved."),
        ],
        {
            "did": "Consolidated and indexed the business discussion for later reuse.",
            "why": "You asked to avoid repeating the same research and decisions next time.",
            "working": "The saved reference links conclusions, source history and original research receipts.",
            "not_working": "Employment terms and the exact Texas address approval remain unresolved.",
            "next": "Read the reference when this topic returns; check changed rules and remaining gaps before action.",
        },
    )
    saved_body = '<p><a href="publishing-reference.md">Read or download the saved business and publishing reference</a>.</p>'
    saved_body += table(["Saved material", "Contents"], [
        ("Company and timing", "LLC ownership, existing employment, business email and when company/store enrollment becomes necessary."),
        ("Store privacy", "Steam, Google and Apple distinctions, United States scope, Northwest evidence and conflicting personal-account guidance."),
        ("Decisions and limits", "Owner facts, tentative preferences, recommendations, unresolved checks and permitted scope stay distinct."),
        ("Research provenance", "Both original reports, review receipts and detailed source history remain linked and preserved."),
        ("Future retrieval", "Task routing, knowledge index, lessons and restart records point to this reference."),
    ])
    saved_body += '<p>This save uses existing findings. It does not claim a fresh policy audit or approval of the proposed address.</p>'
    saved_body += '<p>No company filing, account registration, purchase, message, commit or push started.</p>'
    business_saved += decide.info_thread("aquila-direction-r61-info",
        "Saved a reusable business reference and linked it into future-session startup.",
        body_html=saved_body, short="Business discussion preserved", follows="aquila-direction-r60-info")
    business_saved += decide.whats_next("aquila-direction-r61-next",
        "Use the saved reference when this topic returns, then check only unresolved facts and changed requirements.",
        summary="Nothing needed from you. Native work and business setup remain paused.",
        short="Resume from the saved findings", follows="aquila-direction-r60-next")
    rounds = page.entry(61, "Business discussion saved", business_saved, when="2026-11-16 UTC") + rounds
    expedition_next = decide.recap(
        "Next, I recommend one complete expedition: investigate a signal, fight, collect salvage and improve a turret. "
        "Keep those gains when you return. Then test repeated play on Windows and your Galaxy. "
        "Shared saves remain planned; this gameplay step is proposed, not started.",
        [
            ("Explore", "Choose a nearby signal and let the battleship travel and survey under your orders."),
            ("Fight", "Resolve one encounter through automatic weapons, ammunition choices and positioning orders."),
            ("Improve", "Recover salvage and buy one useful upgrade for a starting turret."),
            ("Keep progress", "Restore the mission and upgrade after closing the game, without duplicate rewards."),
            ("Prove it", "Complete the loop on Windows and Galaxy, then measure sustained device performance."),
        ],
        {
            "did": "Compared the working native prototype with the saved milestone and proposed a complete expedition.",
            "why": "The current prototype needs rewards and lasting ship improvement to make repeated play meaningful.",
            "working": "Navigation, bridge movement, automatic combat, local saves and readable phone controls already exist.",
            "not_working": "Mission rewards, ship progression, hosted saves and sustained performance proof remain missing.",
            "next": "Build the expedition after direction to resume. This proposal starts no implementation or account setup.",
        },
    )
    expedition_body = '<p>Target roughly 10–15 minutes of play. This is a gameplay target, not a development-time estimate.</p>'
    expedition_body += table(["Step", "Player experience"], [
        ("1. Investigate", "Choose a nearby signal, order automatic travel and survey it on arrival."),
        ("2. Fight", "Use navigation and weapon modes during one automatic battle. Traveling rounds, shield hits and armor outcomes stay readable."),
        ("3. Improve", "Recover salvage and spend it on one useful starting-turret upgrade with a clear stat change."),
        ("4. Return", "Close and reopen the game. Keep mission progress and upgrades, then see the improvement in another encounter."),
    ])
    expedition_body += '<p>This connects existing features into a reason to keep playing.</p>'
    expedition_body += '<p>Check mission completion, reward duplication, upgrade effects and save recovery on Windows and your Galaxy.</p>'
    expedition_body += '<p>Then run the planned 20-minute device session for frame pacing, heat and memory. Sustained performance remains unmeasured.</p>'
    expedition_body += '<p>The earlier sequence put shared saves next. I recommend this gameplay proof before choosing a hosted account service.</p>'
    expedition_body += '<p>Shared progress remains required. Company formation is not a technical prerequisite for account or sync design.</p>'
    expedition_body += '<p>The original foundation milestone remains unfinished until shared saves, device proof and its other checks pass.</p>'
    expedition_body += '<p>Business research stays saved. No game changes, purchases, account setup or publication started.</p>'
    expedition_next += decide.info_thread("aquila-direction-r62-info",
        "Next, I recommend a complete expedition that earns a lasting ship upgrade.",
        body_html=expedition_body, short="First complete expedition proposed", follows="aquila-direction-r61-info")
    expedition_next += decide.whats_next("aquila-direction-r62-next",
        "Build the first expedition when development resumes, then verify the complete loop and sustained phone performance.",
        summary="This is a proposed next milestone. Nothing is needed for today's recommendation; implementation remains paused.",
        short="Connect exploration, combat and progress", follows="aquila-direction-r61-next")
    rounds = page.entry(62, "First complete expedition", expedition_next, when="2026-11-16 UTC") + rounds
    address_choice = decide.recap(
        "Saved your employment and outside-work ownership checks as resolved. "
        "The LLC and business address route is chosen. "
        "Prepared focused research comparing Northwest, Texan and iPostal1 for company verification, documents and costs. "
        "No provider selected yet; the prompt is ready for Perplexity.",
        [
            ("Employment", "Recorded your confirmation that creating an LLC poses no conflict with your current employment."),
            ("Ownership", "Recorded your confirmation that work outside employment has no employer ownership issue."),
            ("Company", "Changed the LLC and separate business address from a tentative preference to your chosen direction."),
            ("Providers", "Compare exact Northwest, Texan and iPostal1 plans, including documents, permitted use, reliability and full costs."),
            ("Research", "Saved the complete Perplexity prompt in Research Console and verified its exact readback."),
        ],
        {
            "did": "Saved your resolved checks and chosen company direction, then prepared the focused provider comparison.",
            "why": "You want the strongest address option for the LLC while keeping your home address private.",
            "working": "The research request covers company accounts and carries forward the facts already settled.",
            "not_working": "No exact provider has established acceptance for our proposed Texas company account yet.",
            "next": "Run the prompt in Perplexity and save its complete report for review.",
        },
    )
    address_body = '<p><a href="publishing-reference.md">Updated saved business reference</a> includes your confirmations and the chosen LLC direction.</p>'
    address_body += '<p>Employment and ownership are resolved by your confirmation. No independent contract review is claimed.</p>'
    address_body += '<p>Yes, these services can supply real street addresses while you handle mail remotely.</p>'
    address_body += '<p>The comparison now checks company use and verification documents for the exact plan.</p>'
    address_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google requires the organization’s payments name and address to match Dun &amp; Bradstreet exactly.</a></p>'
    address_body += table(["Candidate", "Current advertised terms", "Research focus"], [
        ("Northwest", '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Texas address: $49/year for 15 scans; $99/year for 20 scans and an office lease.</a>', "Exact company documents, permitted uses, service bundle and renewal costs."),
        ("Texan", '<a href="https://www.texasregisteredagent.net/texas-business-address/texas-virtual-office/">Austin office: $19/month, suite, unlimited scans, lease and phone.</a>', "Company proof and total costs, including separate formation and registered-agent service."),
        ("iPostal1", '<a href="https://ipostal1.com/virtual-business-address-plans-pricing.php">Business plans start at $14.99/month or $149/year. Location prices vary; content scans cost extra.</a>', "Specific Texas facility, applicable terms, documents, mail handling and registered-agent options."),
    ])
    address_body += '<p>Published offers are preliminary checks. No checkout, lease review or Google approval completed.</p>'
    address_body += '<p>The research asks whether any provider is clearly best, then ranks the strongest supported fit if evidence remains incomplete.</p>'
    address_body += '<p>No company filing, purchase, signup or outside contact started. Game implementation remains paused.</p>'
    address_choice += decide.info_thread("aquila-direction-r63-info",
        "Saved your employment checks as resolved and changed the business direction to an LLC with a separate address.",
        body_html=address_body, short="Resolved checks and chosen LLC path", follows="aquila-direction-r62-info")
    address_request = '<p><a href="http://127.0.0.1:8776/?project=aquila-code-dark#31c10ef813054b3fb48a080b0fc3a5ac">Open the provider comparison request</a>.</p>'
    address_request += table(["Step", "Action"], [
        ("1", "Copy the complete prompt into Perplexity and run the research."),
        ("2", "Return the complete Markdown report to this Console request and choose Save result."),
        ("3", "Tell me the research is ready so I can read and compare its evidence."),
    ])
    address_request += '<p>The saved prompt matched the retrieved bytes. No result has arrived yet.</p>'
    address_choice += decide.request("aquila-direction-r63-run",
        "Please run this company-address comparison in Perplexity, then save the complete result in its Research Console request.",
        summary="Research Console stores prompts and results. It cannot execute Perplexity.",
        info=address_request, short="Run the provider comparison",
        reply="Have you saved the complete report in this request?")
    address_choice += decide.whats_next("aquila-direction-r63-next",
        "Review the returned evidence, recommend the best supported plan and resolve any remaining document questions before purchase.",
        summary="Awaiting the research result. Employment and outside-work ownership checks stay closed unless facts change.",
        short="Choose the company address package", follows="aquila-direction-r62-next")
    rounds = page.entry(63, "LLC path and address providers", address_choice, when="2026-11-16 UTC") + rounds
    research_received = decide.recap(
        "I had not retrieved this new comparison before your message. It is saved now, and I downloaded an exact copy. "
        "Console records show one version and no removals. I cannot tell why the earlier upload appeared unsaved. "
        "Full review remains pending.",
        [
            ("Earlier reports", "The previous broad and personal-account research had already been reviewed; this new comparison had not been retrieved."),
            ("Saved now", "The current request contains version 1, saved November 16 at 11:30 PM Central."),
            ("Preserved", "Downloaded all 94,421 bytes and verified the copy matches the saved report."),
            ("History", "This request lists one saved version and no removed versions; the earlier upload issue remains unexplained."),
            ("Review", "Only the opening has been inspected. Full reading and source checks remain pending."),
        ],
        {
            "did": "Checked the saved-version history and retrieved the exact report into project storage.",
            "why": "You wanted to know whether I already had the research after an upload appeared unsaved.",
            "working": "The current report is saved and its retrieved copy matches.",
            "not_working": "The earlier save issue is undiagnosed; the report has not received a full review.",
            "next": "Read the full comparison and check its sources. No further upload is needed.",
        },
    )
    received_body = '<p><a href="http://127.0.0.1:8776/?project=aquila-code-dark#31c10ef813054b3fb48a080b0fc3a5ac">The provider comparison is saved in Research Console</a>.</p>'
    received_body += table(["Check", "Observed result"], [
        ("Saved version", "Version 1, November 16 at 11:30 PM Central."),
        ("Local preservation", "94,421 bytes retrieved; the recorded and local hashes match."),
        ("Earlier versions", "No earlier saved or removed version appears under this request."),
        ("Review state", "Not reviewed. Only the opening was inspected during this receipt check."),
    ])
    received_body += '<p>I cannot establish why the earlier upload appeared unsaved. The current history does not prove deletion or an application bug.</p>'
    research_received += decide.info_thread("aquila-direction-r64-info",
        "I had not retrieved this new comparison before your message. I have it now.",
        body_html=received_body, short="Research receipt confirmed", follows="aquila-direction-r63-info")
    research_received += decide.whats_next("aquila-direction-r64-next",
        "Read the full comparison and check its key sources before recommending a provider.",
        summary="No further upload needed. Retrieval is complete; substantive review remains pending.",
        short="Review the received comparison", follows="aquila-direction-r63-next")
    rounds = page.entry(64, "Research received", research_received, when="2026-11-17 UTC") + rounds
    provider_results = decide.recap(
        "Northwest looks like the best practical fit. Its suggested package totals $438 initially and $224 yearly, before extras. "
        "Texan is the backup; iPostal1 depends more on the local mail center. "
        "No exact Texas plan has proven Google acceptance. Address documents and Texas public-record privacy still need checking.",
        [
            ("First choice", "Northwest ranks first for combined formation, agent service and low-volume mail costs."),
            ("Base budget", "$438 first year and $224 yearly for the suggested services, excluding processing charges and other extras."),
            ("Alternatives", "Texan is the fallback. iPostal1 has more location-specific services and document questions."),
            ("Google", "Company documents must match the verified identity; an office lease alone does not establish address eligibility."),
            ("Privacy", "Texas public reports separately ask where records and daily operations are located."),
        ],
        {
            "did": "Read the complete report, checked its core sources and saved the accepted findings and remaining gaps.",
            "why": "You asked which provider the research favors and what the results mean for your LLC.",
            "working": "Northwest has the strongest practical fit among the compared routes, with a clear base budget.",
            "not_working": "The exact Texas address has no established Google acceptance, and public-record privacy still needs an actual filing check.",
            "next": "Obtain the exact address, agreement and quote, then clarify its company-verification use before purchase.",
        },
    )
    results_body = table(["Rank", "Provider", "Finding"], [
        ("1", "Northwest", '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Best provisional fit: low-volume mail, an advertised lease option and consolidated services.</a> Acceptance remains unproven.'),
        ("2", "Texan", '<a href="https://www.texasregisteredagent.net/texas-business-address/texas-virtual-office/">Backup: $19/month office with unlimited scans, lease and phone, plus $35/year agent.</a> Formation costs are separate.'),
        ("3", "iPostal1", '<a href="https://ipostal1.com/virtual-business-address-plans-pricing.php">Business mailbox starts at $149/year; agent and content scans cost extra.</a> Documents depend on the selected location.'),
    ])
    results_body += '<p>This ranking concerns practical fit and cost. The research did not establish comparative Google approval rates.</p>'
    results_body += table(["Northwest base budget", "Included components"], [
        ("First year: $438", "$300 statutory formation fee, $39 formation service and $99 address/lease plan. First registered-agent year included."),
        ("Following years: $224", "$125 agent plus $99 address/lease at current advertised rates."),
    ])
    results_body += '<p>Sources: <a href="https://www.sos.texas.gov/corp/instructions/205.shtml">Texas fee</a>, <a href="https://www.northwestregisteredagent.com/llc/texas">formation offer</a>, <a href="https://www.northwestregisteredagent.com/registered-agent/texas">agent price</a> and <a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">address plan</a>.</p>'
    results_body += '<p>These are calculated base costs, excluding processing, forwarding, extra documents and other business costs. Obtain an itemized checkout quote.</p>'
    results_body += '<p>The cheaper address tier could reduce those totals to $388 and $174 if its documents suffice.</p>'
    results_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/15633622?hl=en&amp;co=GENIE.CountryCode=US">Google accepts listed IRS/state evidence and representative ID for initial organization verification.</a> An office lease is not required on that list.</p>'
    results_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google requires company name and address to match Dun &amp; Bradstreet.</a></p>'
    results_body += '<p>The unresolved question is whether the exact provider address qualifies for your home-operated company’s matching records.</p>'
    results_body += '<p>A physical street address or lease does not answer that question alone.</p>'
    results_body += '<p><a href="https://comptroller.texas.gov/help/franchise/public-info-report.php?category=taxes">Texas public reports ask where records and daily operations are located.</a> Those fields need a separate home-address privacy check.</p>'
    results_body += '<p>The report found a Northwest-linked Wyoming company example on Google Play. Its exact plan and verification documents remain unknown.</p>'
    results_body += '<p><a href="https://zensoul.app/">Company disclosure</a> and <a href="https://play.google.com/store/apps/details?gl=US&amp;hl=en-US&amp;id=app.zensoul.app">matching Play listing</a>. This does not establish the proposed Texas outcome.</p>'
    results_body += '<p>Read all 296 lines and checked core primary sources. Some state pages blocked direct retrieval; official indexed results supported their fees.</p>'
    results_body += '<p><a href="publishing-reference.md">Saved findings and source limits</a>. Original report remains unchanged; no filing, purchase or outside contact started.</p>'
    provider_results += decide.info_thread("aquila-direction-r65-info",
        "Northwest is the best practical fit found; exact Google acceptance remains unproven.",
        body_html=results_body, short="Provider comparison reviewed", follows="aquila-direction-r64-info")
    provider_results += decide.whats_next("aquila-direction-r65-next",
        "Get Northwest’s exact Texas address, agreement specimen and itemized quote, then clarify D&B and Google eligibility before purchase.",
        summary="Written inquiry drafts are saved. Nothing needed for this results explanation; no contact or purchase started.",
        short="Confirm the exact company-address package", follows="aquila-direction-r64-next")
    rounds = page.entry(65, "Address research results", provider_results, when="2026-11-17 UTC") + rounds
    package_check = decide.recap(
        "I checked Northwest's live order form. The Texas lease, suite and 20-scan plan shows $99/year. "
        "Suites are assigned after signup, so the exact location and documents need a written reply. "
        "The inquiry is ready. Sending waits on your contact name, reply email and permission.",
        [
            ("Checked", "The Texas business-address plan shows $99 at the service-selection stage."),
            ("Address", "Northwest says suites are assigned after signup; the inspected screen showed no assigned Texas address."),
            ("Prepared", "The inquiry requests the address, sample agreement, permitted uses, full costs and service terms."),
            ("Needed", "Sending requires your chosen contact name, reply email and permission; no home address is requested."),
            ("Limit", "Northwest can confirm its services; Google and D&B decide their own acceptance."),
        ],
        {
            "did": "Inspected the live service form and contact route, then prepared the exact pre-purchase inquiry.",
            "why": "You asked whether we can confirm the actual address package before paying.",
            "working": "The advertised plan appears in checkout, and a public inquiry form is available without an account.",
            "not_working": "The assigned address, sample agreement and complete bundle quote remain unconfirmed.",
            "next": "Send the prepared inquiry after you provide contact details and explicit permission.",
        },
    )
    package_body = table(["Check", "Observed result"], [
        ("Texas plan", '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">$99/year includes an office lease, unique suite and 20 mail scans.</a>'),
        ("Live form", '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/signup?mfvo=vo&amp;st=TX">Selected Business and Texas; service-stage order summary showed $99.</a> No identity or payment details entered.'),
        ("Assignment", '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office">Northwest says suites arrive within two business days after signup.</a> The inspected screen showed no assigned address.'),
        ("Written details", "Prepared questions covering the office location, suffix format, specimen agreement, company evidence and permitted uses."),
        ("Full quote", "Requested formation, agent, address, renewal and extra charges, plus refunds and location-change support."),
    ])
    package_body += '<p>The $99 screen is a standalone service estimate. It does not confirm the complete formation bundle or every fee.</p>'
    package_body += '<p>A final suite might only exist after signup. The inquiry requests the office location and sample format before payment.</p>'
    package_body += '<p>No provider reply or sample agreement obtained. Northwest cannot decide Google or D&B acceptance.</p>'
    package_body += '<p><a href="northwest-inquiry.txt">Read the prepared inquiry</a>. <a href="northwest-package-check.json">Inspection record</a>. <a href="publishing-reference.md">Saved business reference</a>.</p>'
    package_check += decide.info_thread("aquila-direction-r66-info",
        "Checked the live $99 Texas plan and prepared the written inquiry needed to confirm its exact details.",
        body_html=package_body, short="Exact package checked; inquiry prepared", follows="aquila-direction-r65-info")
    inquiry_info = '<p><a href="northwest-inquiry.txt">Review the complete message</a> for <a href="https://www.northwestregisteredagent.com/contact-us">Northwest’s inquiry form</a>.</p>'
    inquiry_info += '<p>The form needs first name, last name, email and message. No home address is requested.</p>'
    inquiry_info += '<p>The message contains no employer details, home address or invented company name. It explicitly says this is not an order.</p>'
    package_check += decide.request("aquila-direction-r66-contact",
        "May I send this inquiry to Northwest, and which contact name and reply email should I use?",
        summary="My developer instructions require explicit permission before sending outside messages. The complete message is ready for review.",
        info=inquiry_info, short="Contact details and send permission",
        reply="May I send the prepared inquiry, and what contact name and reply email should I use?")
    package_check += decide.whats_next("aquila-direction-r66-next",
        "Send the inquiry when contact details and permission arrive, then review Northwest’s written response and documents.",
        summary="Nothing purchased, filed or sent. Exact address verification remains incomplete; game work stays paused.",
        short="Obtain the written package confirmation", follows="aquila-direction-r65-next")
    rounds = page.entry(66, "Confirming the exact address package", package_check, when="2026-11-17 UTC") + rounds
    softer_inquiry = decide.recap(
        "Rewrote the email with a warmer, more collegial tone. "
        "It now asks for help with polite questions and closes with thanks. "
        "All five topics remain covered. The revised draft is saved; nothing has been sent.",
        [
            ("Opening", "Added a friendly introduction explaining the business and interest in their services."),
            ("Questions", "Changed directives into polite requests using could you please and would you be able to."),
            ("Language", "Replaced formal specimen wording with sample documents and clearer questions."),
            ("Details", "Kept the address, documents, permitted uses, costs and service terms covered."),
            ("Status", "Saved the revised draft and preserved the original; nothing sent."),
        ],
        {
            "did": "Rewrote the provider inquiry in a warmer, more collegial tone.",
            "why": "You want the email to invite helpful answers without sounding demanding.",
            "working": "Polite questions and appreciation now carry the same information requests.",
            "not_working": "No provider reply exists; contact details and permission to send remain pending.",
            "next": "The revised draft is ready to read. Sending remains a separate pending step.",
        },
    )
    softer_body = '<p><a href="northwest-inquiry-v2.txt">Read the revised email</a>.</p>'
    softer_body += '<p>The opening asks for help confirming details. Each numbered point uses a polite question.</p>'
    softer_body += '<p>Sample documents replaces formal specimen wording. The closing thanks the recipient for their time and help.</p>'
    softer_body += '<p>All five topics remain covered. The original draft is preserved; no message sent.</p>'
    softer_inquiry += decide.info_thread("aquila-direction-r67-info",
        "Rewrote the inquiry with polite questions, a warmer opening and a thank-you.",
        body_html=softer_body, short="Collegial inquiry saved", follows="aquila-direction-r66-info")
    softer_inquiry += decide.request("aquila-direction-r67-contact",
        "When you are ready, may I send the revised inquiry using your chosen contact name and reply email?",
        summary="This remains pending from the previous round. My developer instructions require explicit permission before sending outside messages.",
        info='<p><a href="northwest-inquiry-v2.txt">The revised message</a> is ready. No contact details are needed to finish this wording change.</p>',
        short="Pending contact details and send permission",
        reply="What contact name and reply email should I use if you want the revised inquiry sent?",
        follows="aquila-direction-r66-contact")
    softer_inquiry += decide.whats_next("aquila-direction-r67-next",
        "The wording change is complete. Sending waits on your chosen contact details and instruction to send.",
        summary="No additional research, purchase or company setup started.",
        short="Revised draft ready", follows="aquila-direction-r66-next")
    rounds = page.entry(67, "A more collegial inquiry", softer_inquiry, when="2026-11-17 UTC") + rounds
    inquiry_timing = decide.recap(
        "Added six to twelve months as a tentative timeframe. "
        "The opening now says you are gathering information and are not ready to sign up. "
        "This sets expectations without committing to a date. The email remains unsent.",
        [
            ("Timing", "The opening gives six to twelve months as a tentative timeframe."),
            ("Intent", "It explicitly says you are gathering information and are not ready to sign up."),
            ("Tone", "The friendly wording and polite questions remain."),
            ("Scope", "This is correspondence wording, not a committed company formation or game release date."),
            ("Status", "Saved the new draft and preserved earlier versions; nothing sent."),
        ],
        {
            "did": "Added tentative timing and a clear statement that you are not ready to sign up.",
            "why": "You want Northwest to understand this is advance planning rather than an immediate purchase inquiry.",
            "working": "The opening now sets that expectation directly and politely.",
            "not_working": "The wording cannot guarantee that sales follow-ups will stop.",
            "next": "The draft is updated. Sending still waits on your contact details and instruction.",
        },
    )
    timing_body = '<p><a href="northwest-inquiry-v3.txt">Read the updated email</a>.</p>'
    timing_body += '<p>My tentative timeframe is six to twelve months.</p>'
    timing_body += '<p>I’m gathering information for now and am not ready to sign up.</p>'
    timing_body += '<p>Use this range if it reflects your likely timing. It remains an estimate, not a commitment.</p>'
    inquiry_timing += decide.info_thread("aquila-direction-r68-info",
        "Added a tentative timeframe and made the information-gathering stage explicit.",
        body_html=timing_body, short="Planning timeframe added", follows="aquila-direction-r67-info")
    inquiry_timing += decide.request("aquila-direction-r68-contact",
        "When you are ready, may I send the revised inquiry using your chosen contact name and reply email?",
        summary="This earlier request remains pending. My developer instructions require explicit permission before sending outside messages.",
        info='<p><a href="northwest-inquiry-v3.txt">Current draft</a>. Nothing further is needed for this wording change.</p>',
        short="Pending contact details and send permission",
        reply="What contact name and reply email should I use if you want the revised inquiry sent?",
        follows="aquila-direction-r67-contact")
    inquiry_timing += decide.whats_next("aquila-direction-r68-next",
        "The timing wording is saved. Sending waits on contact details and your instruction.",
        summary="No signup or purchase started. The timeframe is tentative.",
        short="Updated draft ready", follows="aquila-direction-r67-next")
    rounds = page.entry(68, "Setting expectations about timing", inquiry_timing, when="2026-11-17 UTC") + rounds
    shorter_timing = decide.recap(
        "Changed the tentative timeframe to three to six months. The rest of the email stays the same. Nothing sent.",
        [
            ("Timeframe", "Changed the estimate to three to six months."),
            ("Tentative", "The wording still makes clear that timing is an estimate."),
            ("Intent", "The opening still says you are gathering information and are not ready to sign up."),
            ("Draft", "Saved the current version and preserved earlier drafts."),
            ("Sending", "Contact details and your instruction to send remain pending."),
        ],
        {
            "did": "Changed the tentative timeframe to three to six months.",
            "why": "You prefer the shorter planning window.",
            "working": "The revised draft keeps the same polite questions and information-gathering intent.",
            "not_working": "No provider response exists; the email remains unsent.",
            "next": "The edit is complete. Sending remains pending.",
        },
    )
    shorter_timing += decide.info_thread("aquila-direction-r69-info",
        "Changed the tentative timeframe to three to six months.",
        body_html='<p>My tentative timeframe is three to six months.</p><p><a href="northwest-inquiry-v4.txt">Read the current email</a>.</p>',
        short="Timeframe updated", follows="aquila-direction-r68-info")
    shorter_timing += decide.request("aquila-direction-r69-contact",
        "When you are ready, may I send the revised inquiry using your chosen contact name and reply email?",
        summary="This earlier request remains pending. My developer instructions require explicit permission before sending outside messages.",
        info='<p><a href="northwest-inquiry-v4.txt">Current draft</a>. Nothing further is needed for this edit.</p>',
        short="Pending contact details and send permission",
        reply="What contact name and reply email should I use if you want the revised inquiry sent?",
        follows="aquila-direction-r68-contact")
    shorter_timing += decide.whats_next("aquila-direction-r69-next",
        "The timeframe is updated. Sending waits on contact details and your instruction.",
        summary="No signup or purchase started.", short="Current draft ready", follows="aquila-direction-r68-next")
    rounds = page.entry(69, "Three to six months", shorter_timing, when="2026-11-17 UTC") + rounds
    sending_route = decide.recap(
        "Use Northwest's contact form. Enter your first and last name, reply email and the saved message, then choose Send Inquiry. "
        "Company name and phone can stay blank. I opened the form; nothing has been sent.",
        [
            ("Route", "Northwest has a public contact form for the inquiry."),
            ("Details", "It needs first name, last name, email and message."),
            ("Draft", "Paste the current three-to-six-month version into Message, including its subject line."),
            ("Submit", "Choose Send Inquiry after reviewing the completed form."),
            ("Status", "The form is open and blank; no submission made."),
        ],
        {
            "did": "Opened Northwest's contact form and checked its fields.",
            "why": "You asked how to send the prepared email.",
            "working": "The form is available without creating an account or purchasing a service.",
            "not_working": "Nothing has been sent, and actual delivery remains untested.",
            "next": "Enter contact details and submit the draft, or provide those details and instruct me to send it.",
        },
    )
    route_body = table(["Step", "Action"], [
        ("1", '<a href="https://www.northwestregisteredagent.com/contact-us">Open Northwest’s contact form</a> and enter your first name, last name and reply email.'),
        ("2", '<a href="northwest-inquiry-v4.txt">Copy the current draft</a> into Message. Include the subject line there; the form has no separate subject field.'),
        ("3", "Review the form and choose Send Inquiry."),
    ])
    route_body += '<p>Company name and phone have no required marker and can stay blank. No home-address field appears.</p>'
    route_body += '<p>I opened and checked the form. All fields remain blank; no inquiry submitted.</p>'
    sending_route += decide.info_thread("aquila-direction-r70-info",
        "Send the message through Northwest’s public contact form.",
        body_html=route_body, short="Contact form ready", follows="aquila-direction-r69-info")
    sending_route += decide.request("aquila-direction-r70-contact",
        "May I submit the saved message using your preferred first name, last name and reply email?",
        summary="For me to submit it, provide those details and say send it. You can also use the form yourself.",
        info='<p>My developer instructions require explicit permission before sending outside messages.</p><p><a href="northwest-inquiry-v4.txt">Review the exact message</a>.</p>',
        short="Details for agent submission",
        reply="What first name, last name and reply email should I use, and may I submit the inquiry?",
        follows="aquila-direction-r69-contact")
    sending_route += decide.whats_next("aquila-direction-r70-next",
        "Submit the inquiry when contact details and sending instructions are supplied, then inspect the confirmation.",
        summary="No company signup or purchase involved in this inquiry.",
        short="Send the prepared inquiry", follows="aquila-direction-r69-next")
    rounds = page.entry(70, "How to send the inquiry", sending_route, when="2026-11-17 UTC") + rounds
    filled_inquiry = decide.recap(
        "Filled your name, email and saved message. Phone and company stay blank. Nothing submitted.",
        [
            ("Name", "Entered Jacob Mott."),
            ("Email", "Entered the reply email you supplied."),
            ("Message", "The full message matches the saved version with tentative three-to-six-month timing."),
            ("Optional fields", "Phone and company remain blank as requested."),
            ("Status", "The completed form remains open and unsent."),
        ],
        {
            "did": "Filled the requested contact details and message.",
            "why": "You supplied your email and asked me to fill the form.",
            "working": "Readback confirms every entered value and both blank optional fields.",
            "not_working": "No inquiry submitted; actual delivery remains untested.",
            "next": "Filling is complete. Sending waits on your instruction.",
        },
    )
    filled_body = table(["Check", "Result"], [
        ("Contact details", "Name and email match the intended values."),
        ("Message", '<a href="northwest-inquiry-v4.txt">Saved message</a> matches all 2,710 characters, including the subject line.'),
        ("Phone and company", "Both blank."),
        ("Submission", "Send Inquiry remains available; it was not clicked."),
    ])
    filled_body += '<p>No confirmation or email delivery claimed.</p>'
    filled_inquiry += decide.info_thread("aquila-direction-r71-info",
        "The contact form is filled and verified.",
        body_html=filled_body, short="Form filled, unsent", follows="aquila-direction-r70-info")
    filled_inquiry += decide.request("aquila-direction-r71-contact",
        "May I submit the completed inquiry to Northwest?",
        summary="Contact details are supplied. My developer instructions require explicit permission before sending outside messages.",
        info='<p>Your request authorized filling. <a href="northwest-inquiry-v4.txt">Review the exact message</a> before sending.</p>',
        short="Send instruction pending",
        reply="May I click Send Inquiry?",
        follows="aquila-direction-r70-contact")
    filled_inquiry += decide.whats_next("aquila-direction-r71-next",
        "Filling is complete. Submission waits on your instruction, followed by a confirmation check.",
        summary="Phone and company remain blank for sending.",
        short="Ready for submission", follows="aquila-direction-r70-next")
    rounds = page.entry(71, "Inquiry filled, not sent", filled_inquiry, when="2026-11-17 UTC") + rounds
    inquiry_confirmed = decide.recap(
        "Northwest confirmed the submission. I checked the same success page and saved the status. We’re waiting for their reply.",
        [
            ("Confirmation", "Your pasted message matches Northwest’s success page."),
            ("Verified", "The existing browser tab shows the same confirmation."),
            ("Saved", "Project records now mark the inquiry submitted."),
            ("Request closed", "No sending permission remains pending, and no duplicate was sent."),
            ("Next", "Await their reply about the address, documents and pricing."),
        ],
        {
            "did": "Verified and recorded the submission confirmation.",
            "why": "You shared Northwest’s success message after submitting the form.",
            "working": "Their confirmation page says a Corporate Guide will be in touch.",
            "not_working": "Their answers and supporting documents are still pending.",
            "next": "Wait for the reply before reviewing the exact package.",
        },
    )
    inquiry_confirmed += decide.info_thread("aquila-direction-r72-info",
        "Northwest’s submission confirmation is verified and saved.",
        body_html='<p>Your pasted confirmation matches the page shown in the existing browser tab.</p>'
            '<p><a href="northwest-inquiry-confirmation.json">Confirmation record</a>. The earlier send request is closed; I did not send a duplicate.</p>'
            '<p>This confirms submission, not their answers about address use or pricing.</p>',
        short="Submission confirmed", follows="aquila-direction-r71-info")
    inquiry_confirmed += decide.whats_next("aquila-direction-r72-next",
        "Await Northwest’s reply, then review the address, agreement and full quote.",
        summary="Nothing needed from you now.",
        short="Waiting for their reply", follows="aquila-direction-r71-next")
    rounds = page.entry(72, "Northwest inquiry submitted", inquiry_confirmed, when="2026-11-17 UTC") + rounds
    return_to_game = decide.recap(
        "Next, build one complete expedition: investigate a signal, fight, collect salvage and upgrade a turret. "
        "Keep your gains after reopening the game. Then test repeated play on Windows and your Galaxy. "
        "Northwest’s reply can wait alongside this work; game development remains paused until you choose to resume.",
        [
            ("Investigate", "Choose a nearby signal and order automatic travel and a survey."),
            ("Fight", "Command positioning and weapon modes while the ship aims and fires automatically."),
            ("Improve", "Recover salvage and spend it on one useful turret upgrade."),
            ("Keep progress", "Save mission progress, rewards and upgrades, then continue after reopening."),
            ("Test", "Complete the loop on Windows and Galaxy, then measure a 20-minute phone session."),
        ],
        {
            "did": "Checked the saved expedition proposal against the current native foundation.",
            "why": "A complete loop connects exploration, tactical decisions and lasting ship improvement.",
            "working": "Automatic navigation, combat, local saves and readable phone controls already exist.",
            "not_working": "Missions, salvage, upgrades and sustained performance proof are still missing.",
            "next": "Resume with the first expedition if you accept this scope and order.",
        },
    )
    return_body = '<p>Build one expedition lasting roughly 10–15 minutes of play.</p>'
    return_body += table(["Step", "Player experience"], [
        ("Investigate", "Choose a nearby signal; your battleship travels and surveys under your orders."),
        ("Fight", "Choose positioning, ammunition and weapon modes during one automatic battle."),
        ("Upgrade", "Collect salvage and improve one starting turret, with a visible stat change."),
        ("Continue", "Close and reopen the game; keep progress and see the upgrade affect the next encounter."),
    ])
    return_body += '<p>Check completion, reward duplication, upgrade effects and save recovery on Windows and Galaxy.</p>'
    return_body += '<p>Then measure frame pacing, heat and memory during the planned 20-minute phone session.</p>'
    return_body += '<p>Shared accounts, synchronized saves and Apple proof remain required later. The original foundation milestone is still incomplete.</p>'
    return_body += '<p>Northwest’s pending reply does not block this local work. This is a recommendation; no game changes started.</p>'
    return_to_game += decide.info_thread("aquila-direction-r73-info",
        "I recommend returning to the game with its first complete expedition.",
        body_html=return_body, short="First expedition next", follows="aquila-direction-r72-info")
    return_to_game += decide.request("aquila-direction-r73-resume",
        "Shall we resume game development with this first expedition?",
        summary="Game work is paused from our business discussion. This asks to resume the scope above.",
        info='<p>I recommend this gameplay step before shared account setup, followed by sustained device checks.</p>',
        short="Resume the proposed expedition",
        reply="Should I build this expedition next?")
    return_to_game += decide.whats_next("aquila-direction-r73-next",
        "After your direction to resume, connect the expedition, rewards, upgrade and saved progress, then test the loop.",
        summary="The business inquiry remains submitted; await Northwest’s reply separately.",
        short="Resume a complete play loop", follows="aquila-direction-r72-next")
    rounds = page.entry(73, "Back to the first expedition", return_to_game, when="2026-11-17 UTC") + rounds
    expedition = decide.recap(
        "Windows is our main development environment. Keep the phone disconnected during routine work. "
        "We’ll batch phone checks at milestones or for phone-specific fixes. "
        "The expedition, salvage, turret refit and saves are built. Windows and Android packages pass; new physical input checks remain.",
        [
            ("Main environment", "Use Windows for coding, debugging and ordinary playtests."),
            ("Phone", "Connect for milestone checks, relevant mobile changes or phone-specific bugs."),
            ("Expedition", "Investigate a signal, survey, fight automatically, recover salvage and improve one turret."),
            ("Verified", "Eleven simulation/save groups, both packages and isolated Windows rendering pass."),
            ("Still open", "New physical input, human pacing, sustained performance, shared saves and Apple proof remain."),
        ],
        {
            "did": "Built the first expedition and saved the Windows-first workflow.",
            "why": "You approved resuming and questioned keeping the phone connected during development.",
            "working": "Mission progress, salvage and refit survive saves; the refit changes projectile damage.",
            "not_working": "Native window controls are unavailable; new mouse/touch input and physical-phone performance remain untested.",
            "next": "Playtest commands and pacing on Windows, then batch phone checks at a suitable milestone.",
        },
    )
    expedition_body = '<p>Windows is the primary development environment. The Galaxy can stay disconnected during routine work.</p>'
    expedition_body += '<p>Connect for playable milestones, relevant mobile changes or phone-specific bugs. Android compilation and touch previews need no phone.</p>'
    expedition_body += table(["Built", "Behavior"], [
        ("Signal and survey", "Automatic travel to a seeded signal, followed by a 90-second survey."),
        ("Combat", "Automatic acquisition and projectile fire; player chooses positioning and ammunition."),
        ("Recovery", "Approach the wreck; 45-second recovery grants 120 salvage once."),
        ("Refit", "Spend 100 salvage for +50% port-turret impact damage; starboard stays unchanged."),
        ("Progress", "Mission, rewards and refit persist. Practice cannot reset an active expedition."),
    ])
    expedition_body += table(["Evidence", "Result and limit"], [
        ("Native tests", "All 11 simulation/save groups pass, including duplicate rewards, refit impact, legacy saves and frame-rate parity."),
        ("Packages", "Editor, Windows and Android builds pass. Android signature, embedded content and archive integrity pass."),
        ("Windows runtime", "Seven rendered views inspected. Packaged relaunch retains one completion, 20 salvage and the upgraded turret."),
        ("Pacing", "Baseline simulation takes 547.35 seconds. The 10–15-minute player-time target still needs human playtesting."),
        ("Input", "Native control helper is unavailable. Scene captures do not prove mouse or touch input."),
        ("Phone", "This expedition build was not installed or played on the Galaxy. Sustained performance remains unmeasured."),
    ])
    expedition_body += '<p><a href="expedition-verification.json">Current verification record</a>. Earlier phone evidence remains historical.</p>'
    expedition_body += page.figure(page.copy_media(HERE / "media/r74-desktop-available-1600.png", out),
        "74.1 Desktop commands.", "Packaged Windows rendering; isolated save. The expedition starts from the upper-right panel.")
    expedition_body += page.figure(page.copy_media(HERE / "media/r74-desktop-battle.png", out),
        "74.2 Automatic battle.", "Prepared battle fixture; real projectile simulation and rendering. No input injection.")
    expedition_body += page.figure(page.copy_media(HERE / "media/r74-desktop-upgraded-reload.png", out),
        "74.3 Refit survives relaunch.", "Completed simulation journal, reopened twice in the packaged game. Balance and upgraded mount remain.")
    expedition_body += page.figure(page.copy_media(HERE / "media/r74-touch-salvage.png", out),
        "74.4 Touch layout preview.", "Windows rendering at phone dimensions, using a prepared recovery fixture. Actual Galaxy testing remains pending.")
    expedition_body += page.figure(page.copy_media(HERE / "media/r74-touch-bridge.png", out),
        "74.5 Bridge layout preview.", "Movement controls and expedition menu fit together. Windows preview only; physical touch remains untested.")
    expedition_body += '<p>Changes remain local and uncommitted. Northwest’s reply is still pending; it does not block development.</p>'
    expedition += decide.info_thread("aquila-direction-r74-info",
        "Windows leads development; the first expedition is built.",
        body_html=expedition_body, short="Windows-first expedition", follows="aquila-direction-r73-info")
    expedition += decide.whats_next("aquila-direction-r74-next",
        "Playtest expedition commands and pacing on Windows, then batch Galaxy checks when convenient.",
        summary="No phone connection needed now. Shared accounts, cloud saves and Apple proof remain later requirements.",
        short="Windows playtest next", follows="aquila-direction-r73-next")
    rounds = page.entry(74, "Windows first; expedition built", expedition, when="2026-11-17 UTC") + rounds
    workflow = decide.recap(
        "Saved your agreement. Windows handles daily development and playtesting. "
        "We’ll install and test phone builds occasionally, at milestones or for mobile-specific changes. "
        "No continuous phone connection needed.",
        [
            ("Daily work", "Windows remains the main development and playtest environment."),
            ("Phone checks", "Install and test Galaxy builds occasionally."),
            ("Timing", "Batch checks at milestones or for mobile-specific changes and bugs."),
            ("Saved", "Project decisions, knowledge, plan and restart notes record your explicit agreement."),
            ("Next", "Continue the existing Windows expedition validation."),
        ],
        {
            "did": "Saved your confirmed development workflow in the project records.",
            "why": "You agreed to occasional phone testing and asked to preserve that choice.",
            "working": "Future sessions can recover this decision from the checkpoint and shared knowledge.",
            "not_working": "New expedition physical input and sustained performance remain untested.",
            "next": "Continue Windows command and pacing checks; batch phone tests when needed.",
        },
    )
    workflow += decide.info_thread("aquila-direction-r75-info",
        "Saved: Windows-first development, with occasional phone testing.",
        body_html='<p>Your agreement is recorded in project decisions, shared knowledge, the development plan and restart notes.</p>'
            '<p>Daily work stays on Windows. Phone builds are installed and tested at milestones or for relevant mobile changes.</p>'
            '<p>The previous checkpoint is archived. This documentation update adds no runtime test evidence.</p>',
        short="Workflow confirmed and saved", follows="aquila-direction-r74-info")
    workflow += decide.whats_next("aquila-direction-r75-next",
        "Continue Windows expedition command and pacing checks, with phone testing in occasional batches.",
        summary="No phone connection needed now.",
        short="Continue on Windows", follows="aquila-direction-r74-next")
    rounds = page.entry(75, "Development workflow confirmed", workflow, when="2026-11-17 UTC") + rounds
    playtest = decide.recap(
        "Next, play one complete expedition on Windows. Investigate, fight, recover salvage and refit the port turret. "
        "Close and reopen to check your progress. Tell me what feels confusing, slow or unsatisfying. "
        "Then fix those findings before longer performance testing. Phone can stay disconnected.",
        [
            ("Start", "Open the Windows build and choose Investigate signal."),
            ("Fight", "Watch automatic combat; fit piercing rounds when armor blocks standard shots."),
            ("Improve", "Recover salvage, then buy the port-turret refit."),
            ("Return", "Save, close and reopen; confirm port Mk II and 20 salvage remain after the first refit."),
            ("Review", "Report unclear controls, slow pacing or weak combat feedback; fix those before longer testing."),
        ],
        {
            "did": "Checked the existing Windows build and made the next playtest concrete.",
            "why": "Automated checks cannot tell us whether the expedition feels clear and enjoyable.",
            "working": "The packaged build exists; prior simulation and save checks pass.",
            "not_working": "Human pacing and new physical input remain unverified.",
            "next": "Play one expedition on Windows, report findings, then fix the problems it reveals.",
        },
    )
    playtest_body = '<p>Use the existing Windows build for one complete playthrough, targeting roughly 10–15 minutes.</p>'
    playtest_body += '<p>Open <code>D:/OtherProjects/AquilaCodeDark/.runtime/native-package/Win64/Aquila.exe</code>.</p>'
    playtest_body += table(["Step", "Check"], [
        ("1. Investigate", "Choose Investigate signal. The ship travels and surveys automatically."),
        ("2. Fight", "Watch weapon fire and shield hits. Fit piercing rounds when armor blocks standard shots."),
        ("3. Refit", "Choose Recover salvage after victory. Spend 100 of the 120 salvage on the port turret."),
        ("4. Reopen", "Save, close and reopen. First-expedition completion, port Mk II and 20 salvage should remain."),
    ])
    playtest_body += '<p>We need feedback on clarity, pacing and how combat feels. Phone testing can wait.</p>'
    playtest_body += '<p>The executable exists; no new game session or runtime test started for this recommendation.</p>'
    playtest += decide.info_thread("aquila-direction-r76-info",
        "Next: one complete Windows expedition playtest.",
        body_html=playtest_body, short="Test the full expedition", follows="aquila-direction-r75-info")
    playtest += decide.request("aquila-direction-r76-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Your feedback checks the experience beyond automated simulation and save tests.",
        short="Windows playthrough feedback",
        reply="What needs fixing, and did your progress survive reopening?")
    playtest += decide.whats_next("aquila-direction-r76-next",
        "Fix issues from the playthrough, then run a longer Windows session before the next phone milestone.",
        summary="This waits on playthrough findings. No phone connection needed now.",
        short="Tune the first expedition", follows="aquila-direction-r75-next")
    rounds = page.entry(76, "Play the Windows expedition", playtest, when="2026-11-17 UTC") + rounds
    northwest_reply = decide.recap(
        "Northwest quotes $438 initially and $224 yearly for the listed services. "
        "Its address handles mail and formation. It warns about D-U-N-S acceptance. "
        "Google publishes the verified company address. The reply is saved; this package is not a confirmed privacy solution.",
        [
            ("Quote", "$438 first year; $224 annual renewal for the named services, at quoted rates."),
            ("Package", "Unique mailing suite, 20 annual mail scans and a one-year lease issued after purchase."),
            ("Warning", "Northwest says D&B does not accept registered-agent or virtual-office addresses."),
            ("Evidence", "Google requires matching company records; older D&B guidance supports caution but does not resolve this exact package."),
            ("Recommendation", "Confirm address eligibility before purchasing for Google Play privacy."),
        ],
        {
            "did": "Saved your pasted reply and checked the relevant official address guidance.",
            "why": "Northwest answered pricing and package questions but warned about the key privacy requirement.",
            "working": "The service quote and mailing terms are documented.",
            "not_working": "This address has no confirmed D&B or Google approval for your planned company.",
            "next": "Clarify this exact setup with D&B and Google before relying on it.",
        },
    )
    northwest_body = '<p>Northwest remains a formation and mail candidate. I would not purchase based on presumed Google Play privacy compatibility.</p>'
    northwest_body += table(["Answered", "Provider reply"], [
        ("First year", "$39 formation + $300 state fee + included agent year + $99 office plan = $438."),
        ("Renewal", "$125 registered agent + $99 office plan = $224 annually. Additional chosen services excluded."),
        ("Mail and documents", "20 opened-and-scanned pieces yearly; unique suite; one-year lease after purchase, with replacement and correction requests allowed."),
        ("Address use", "Texas formation and correspondence, including IRS mail. Mailing address does not replace a required operating location."),
        ("Format and identity", "Use # plus assigned number. Mail handling requires USPS Form 1583, photo ID and separate residential-address proof."),
        ("Unavailable", "No utility bills, in-person verification or verification videos."),
    ])
    northwest_body += '<p>Northwest says D&B rejects registered-agent and virtual-office addresses. That statement comes from Northwest, not a direct D&B response.</p>'
    northwest_body += '<p><a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google requires matching D&B and payments details</a> and publishes the organization’s legal address.</p>'
    northwest_body += '<p>A <a href="https://www.ams.usda.gov/sites/default/files/media/DNB_DUNSLookup_USDA.pdf">D&B-authored 2021 USDA guide</a> excludes virtual offices as physical locations.</p>'
    northwest_body += '<p>Its grant-program context and age limit its use here. It supports caution, not a current ruling on every address service.</p>'
    northwest_body += '<p><a href="https://www.dnb.com/en-us/smb/duns/get-a-duns.html">Current D&B enrollment guidance</a> did not settle this exact Texas package.</p>'
    northwest_body += '<p>Exact street, sample agreement, extra fees, mail retention, refund terms and relocation support remain open.</p>'
    northwest_body += '<p><a href="northwest-package-reply.json">Saved reply review</a>; <a href="publishing-reference.md">updated business reference</a>.</p>'
    northwest_body += '<p>Evidence came from your pasted message. Email headers and transport were not independently checked.</p>'
    northwest_body += '<p>No follow-up sent or purchase made. Employment and outside-work ownership remain resolved; Windows playtest feedback remains pending.</p>'
    northwest_reply += decide.info_thread("aquila-direction-r77-info",
        "Northwest’s package is clearer; Google Play address privacy remains unproven.",
        body_html=northwest_body, short="Northwest reply reviewed", follows="aquila-direction-r76-info")
    northwest_reply += decide.request("aquila-direction-r77-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Carried from the game discussion; answer when you return to playtesting. No phone connection needed.",
        short="Windows feedback still pending", follows="aquila-direction-r76-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    northwest_reply += decide.whats_next("aquila-direction-r77-next",
        "Clarify D&B’s current treatment of this exact company setup, then confirm which verified address Google would publish.",
        summary="Recommend resolving this before purchasing for privacy. Local game work remains independent.",
        short="Resolve address eligibility", follows="aquila-direction-r76-next")
    rounds = page.entry(77, "Northwest replied; privacy unresolved", northwest_reply, when="2026-11-17 UTC") + rounds
    clarification = decide.recap(
        "Northwest did not say Google Play rejects the address. Google makes its own decisions; a lease does not guarantee acceptance. "
        "The firm rejection claim concerned D&B. Linking that to a possible Google problem was my inference, not their confirmed finding.",
        [
            ("Google Play", "They did not say Google Play would reject the address."),
            ("Their wording", "They asked you to check Google’s requirements and said a lease does not guarantee acceptance."),
            ("D&B", "Their categorical exclusion concerns registered-agent and virtual-office addresses for D-U-N-S."),
            ("My inference", "I connected that D&B warning to a possible Google verification problem."),
            ("Correction", "That possibility is not a confirmed Google restriction or rejection."),
        ],
        {
            "did": "Separated Northwest’s exact statements from my interpretation.",
            "why": "My earlier emphasis made a possible Google issue sound more definite than the email supports.",
            "working": "Their Google wording and D&B claim are now distinguished in the saved notes.",
            "not_working": "The email establishes neither Google approval nor Google rejection.",
            "next": "Continue discussing the reply without treating an inference as a confirmed platform decision.",
        },
    )
    clarification_body = '<p>The Google passage says:</p>'
    clarification_body += '<blockquote>Google payments and Google Play make their own address-verification decisions, so please check their requirements for both the public contact address and the actual operating location before purchasing. An office lease does not guarantee acceptance.</blockquote>'
    clarification_body += '<p>The categorical claim concerns D&B:</p>'
    clarification_body += '<blockquote>Dun &amp; Bradstreet does not accept registered-agent or virtual-office addresses.</blockquote>'
    clarification_body += '<p>I inferred a possible Google verification obstacle from that D&B claim. Northwest did not confirm that outcome.</p>'
    clarification_body += '<p>My purchase caution was a recommendation, not evidence that Google rejects the address. I should have kept that distinction clearer.</p>'
    clarification += decide.info_thread("aquila-direction-r78-info",
        "Northwest did not say Google Play would reject this address.",
        body_html=clarification_body, short="Exact wording clarified", follows="aquila-direction-r77-info")
    clarification += decide.request("aquila-direction-r78-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Earlier game request remains pending for when you return to playtesting.",
        short="Earlier Windows feedback request", follows="aquila-direction-r77-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    clarification += decide.whats_next("aquila-direction-r78-next",
        "Keep Google acceptance unresolved while discussing the reply.",
        summary="This clarification starts no new research, contact or purchase.",
        short="Keep the distinction clear", follows="aquila-direction-r77-next")
    rounds = page.entry(78, "What Northwest said about Google", clarification, when="2026-11-17 UTC") + rounds
    example = decide.recap(
        "Yes. Zensoul LLC was our Google Play example. Its website names Northwest; its Play listing shows the matching Wyoming address. "
        "Both pages still match. That evidence remains relevant. The exact address package and private verification documents remain unknown.",
        [
            ("Example", "Zensoul LLC has a live Google Play listing with in-app purchases."),
            ("Northwest link", "Its own website names Northwest Registered Agent Service Inc."),
            ("Address", "The developer website and Play listing show the same Sheridan street and suite."),
            ("Rechecked", "Both primary pages still show those details."),
            ("Limit", "We did not inspect its exact package or private verification documents."),
        ],
        {
            "did": "Rechecked the Northwest-linked app example you remembered.",
            "why": "That positive evidence must remain part of our interpretation of the provider’s reply.",
            "working": "The developer website and Google Play listing still show the matching business address.",
            "not_working": "The example does not reveal its exact service package or verification history.",
            "next": "Keep this example alongside the provider’s warning without claiming a blanket Google prohibition.",
        },
    )
    example_body = '<p>Your recollection is correct. I should have kept this example prominent when discussing the reply.</p>'
    example_body += table(["Evidence", "Observed"], [
        ('<a href="https://zensoul.app/">Zensoul website</a>', 'Names Zensoul LLC, care of Northwest Registered Agent Service Inc, at its Sheridan address.'),
        ('<a href="https://play.google.com/store/apps/details?gl=US&amp;hl=en-US&amp;id=app.zensoul.app">Google Play listing</a>', 'Lists the same company and street/suite address; offers in-app purchases and describes an annual membership.'),
    ])
    example_body += '<p>Matching street address:</p><blockquote>30 N Gould St Ste N, Sheridan, WY</blockquote>'
    example_body += '<p>This is concrete public evidence, not only a developer anecdote. Northwest’s email does not erase it.</p>'
    example_body += '<p>It concerns Wyoming. The exact package and private approval documents remain unknown; no Texas outcome is established.</p>'
    example += decide.info_thread("aquila-direction-r79-info",
        "Yes. Zensoul LLC is the Northwest-linked Google Play example we found.",
        body_html=example_body, short="Example rechecked", follows="aquila-direction-r78-info")
    example += decide.request("aquila-direction-r79-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Earlier game request remains pending for when you return to playtesting.",
        short="Earlier Windows feedback request", follows="aquila-direction-r78-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    example += decide.whats_next("aquila-direction-r79-next",
        "Keep the confirmed public example in view when interpreting Northwest’s reply.",
        summary="The specific Texas package remains unverified; no Google rejection was stated.",
        short="Preserve both pieces of evidence", follows="aquila-direction-r78-next")
    rounds = page.entry(79, "The Northwest-linked app example", example, when="2026-11-17 UTC") + rounds
    evidence_weight = decide.recap(
        "Yes. The listing demonstrates a Northwest-linked business address being used on Google Play. "
        "I gave the missing private paperwork too much weight. Our specific Texas package remains unverified, but the observed example counts.",
        [
            ("Answer", "Yes, the public-address outcome is demonstrated for that developer."),
            ("Proof", "A live Google Play listing uses the business address linked to Northwest on its developer’s website."),
            ("Weight", "Missing private paperwork does not erase the observed result."),
            ("Remaining question", "We have not tested our specific Texas package."),
            ("Correction", "I understated what the existing example establishes."),
        ],
        {
            "did": "Corrected how I described the weight of the app example.",
            "why": "You asked whether an observed working example counts as proof.",
            "working": "The public-address outcome is demonstrated for that developer.",
            "not_working": "Our specific Texas package has not been tested.",
            "next": "Use the observed case as positive evidence when discussing our own setup.",
        },
    )
    evidence_body = '<p>The observed result is an app listed on Google Play using a Northwest-linked business address.</p>'
    evidence_body += '<p><a href="https://zensoul.app/">The developer’s website</a> and <a href="https://play.google.com/store/apps/details?gl=US&amp;hl=en-US&amp;id=app.zensoul.app">its Play listing</a> establish that result.</p>'
    evidence_body += '<p>I gave the missing private paperwork too much weight. It does not undo what those pages show.</p>'
    evidence_body += '<p>The remaining question concerns reproducing that result with our specific Texas package.</p>'
    evidence_weight += decide.info_thread("aquila-direction-r80-info",
        "Yes. It demonstrates the public-address outcome working for that developer.",
        body_html=evidence_body, short="The example counts", follows="aquila-direction-r79-info")
    evidence_weight += decide.request("aquila-direction-r80-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Earlier game request remains pending for when you return to playtesting.",
        short="Earlier Windows feedback request", follows="aquila-direction-r79-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    evidence_weight += decide.whats_next("aquila-direction-r80-next",
        "Treat the example as demonstrated positive evidence when discussing the Texas setup.",
        summary="This clarification adds no purchase, contact or research action.",
        short="Give the example proper weight", follows="aquila-direction-r79-next")
    rounds = page.entry(80, "Yes, the example demonstrates it", evidence_weight, when="2026-11-17 UTC") + rounds
    candidate = decide.recap(
        "Yes. Northwest remains my first choice among the three providers. Its services, cost and the Play example support that recommendation. "
        "The exact Texas verification outcome remains unconfirmed.",
        [
            ("Recommendation", "Northwest remains my first choice among the three providers we compared."),
            ("Fit", "It combines formation, registered-agent service and the address plan at a suitable low-volume cost."),
            ("Evidence", "The written quote and demonstrated Google Play example strengthen that recommendation."),
            ("Confidence", "This ranks overall fit; it does not establish comparative Google approval rates."),
            ("Status", "No purchase or signup occurred."),
        ],
        {
            "did": "Rechecked the provider plan pages and clarified the saved recommendation.",
            "why": "You asked whether Northwest remains the strongest candidate.",
            "working": "The combined services, cost and observed app example still support the ranking.",
            "not_working": "The exact Texas package has no confirmed Google verification outcome.",
            "next": "Keep Northwest as the first-choice candidate while discussing your setup.",
        },
    )
    candidate_body = '<p>Northwest stays ahead of Texan Registered Agent and iPostal1 for your planned low-volume setup.</p>'
    candidate_body += '<p>The <a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Northwest plan</a> combines a lease and 20 annual mail scans for $99 yearly.</p>'
    candidate_body += '<p><a href="https://www.texasregisteredagent.net/texas-business-address/texas-virtual-office/">Texan</a> offers more mail capacity at $19 monthly; <a href="https://ipostal1.com/virtual-business-address-plans-pricing.php">iPostal1</a> business plans start at $149 yearly.</p>'
    candidate_body += '<p>These are different plans, not equivalent service bundles. The quoted Northwest formation and agent terms remain saved.</p>'
    candidate_body += '<p><a href="https://zensoul.app/">Zensoul’s website</a> and <a href="https://play.google.com/store/apps/details?gl=US&amp;hl=en-US&amp;id=app.zensoul.app">Google Play listing</a> demonstrate a Northwest-linked public address in use.</p>'
    candidate_body += '<p>My verification caution did not establish that either alternative was better. Exact Texas acceptance remains unconfirmed.</p>'
    candidate_body += '<p><a href="publishing-reference.md">Saved recommendation</a>; <a href="northwest-package-reply.json">provider reply</a>. No purchase or signup occurred.</p>'
    candidate += decide.info_thread("aquila-direction-r81-info",
        "Yes. Northwest remains my first choice among the three providers we compared.",
        body_html=candidate_body, short="Northwest remains first choice", follows="aquila-direction-r80-info")
    candidate += decide.request("aquila-direction-r81-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Earlier game request remains pending for when you return to playtesting.",
        short="Earlier Windows feedback request", follows="aquila-direction-r80-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    candidate += decide.whats_next("aquila-direction-r81-next",
        "Keep Northwest as the first-choice candidate while discussing the setup.",
        summary="No immediate action needed. This recommendation starts no purchase or signup.",
        short="Recommendation stands", follows="aquila-direction-r80-next")
    rounds = page.entry(81, "Northwest remains my first choice", candidate, when="2026-11-17") + rounds
    alternatives = decide.recap(
        "Yes. A broader comparison can test whether another provider fits better. The full prompt is ready in Research Console. "
        "It includes our three candidates, Northwest’s reply and the working app example. You run it in Perplexity and save the answer.",
        [
            ("Search", "Look beyond Northwest, Texan Registered Agent and iPostal1."),
            ("Compare", "Check Google Play evidence, exact Texas packages, total costs, documents and mail service."),
            ("Context", "Keep the provider’s reply and demonstrated app example in view."),
            ("Status", "The prompt is queued locally; Perplexity has not run through this workflow."),
            ("Next", "Run the prompt and save the complete answer for review."),
        ],
        {
            "did": "Prepared and saved a broader provider comparison request.",
            "why": "You asked whether options beyond the three known providers might be better.",
            "working": "The complete prompt and its matching submission receipt are saved.",
            "not_working": "No new research result or broader winner yet.",
            "next": "You run the prompt in Perplexity; I review the saved answer.",
        },
    )
    alternatives_body = '<p>The request searches for up to six additional Texas options, including separate formation and address providers.</p>'
    alternatives_body += '<p>It compares evidence, exact services and costs without assuming Northwest must win.</p>'
    alternatives_body += '<p>Northwest’s reply and the demonstrated app example are included. US-only planning and settled employment questions remain unchanged.</p>'
    alternatives_body += '<p><a href="http://127.0.0.1:8776/?project=aquila-code-dark#1caac6482b7b4aa4803ae28d63b1c53d">Open the complete research request</a>; <a href="address-alternatives-receipt.json">submission receipt</a>.</p>'
    alternatives_body += '<p>Local readback matches the prompt’s byte hash. Research Console stores the request; it does not execute Perplexity.</p>'
    alternatives_body += '<p>No new result, purchase, signup or provider contact occurred.</p>'
    alternatives += decide.info_thread("aquila-direction-r82-info",
        "The broader provider comparison is ready for you to run in Perplexity.",
        body_html=alternatives_body, short="Broader comparison prepared", follows="aquila-direction-r81-info")
    alternatives += decide.request("aquila-direction-r82-research",
        "Run this comparison in Perplexity, then save its full answer in Research Console.",
        summary="The broader evidence will test whether another provider fits your needs better.",
        info='<p>Open <a href="http://127.0.0.1:8776/?project=aquila-code-dark#1caac6482b7b4aa4803ae28d63b1c53d">the research request</a>, copy its prompt, run it, then import or paste the answer. Press Save result.</p>',
        short="Run the broader comparison",
        reply="Tell me when the complete result is saved.")
    alternatives += decide.request("aquila-direction-r82-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Earlier game request remains pending for when you return to playtesting.",
        short="Earlier Windows feedback request", follows="aquila-direction-r81-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    alternatives += decide.whats_next("aquila-direction-r82-next",
        "Review the saved result and keep or change the provider ranking based on its evidence.",
        summary="Waiting on the Perplexity answer. Current preference remains Northwest among the three already compared.",
        short="Compare the returned evidence", follows="aquila-direction-r81-next")
    rounds = page.entry(82, "Look beyond the three providers", alternatives, when="2026-11-17") + rounds
    reviewed_alternatives = decide.recap(
        "Northwest still looks best for your low-volume start. Texas Virtual Office is a credible alternative with cheaper renewals and unlimited scans. "
        "A paid Texas game uses Northwest’s published address. I corrected several overstatements in the research.",
        [
            ("Recommendation", "Northwest remains preferred for the planned low-volume startup."),
            ("New evidence", "Afterplace’s paid Google Play listing matches Northwest’s published Texas street and suite."),
            ("Alternative", "Texas Virtual Office advertises a lease and unlimited scans; the report missed these inclusions."),
            ("Costs", "Northwest quotes $438 first year and $224 renewal; the alternative totals $615 and $205 before extras."),
            ("Review", "The full original is preserved, source corrections saved, and the returned version marked reviewed."),
        ],
        {
            "did": "Read the complete report and checked the findings most likely to change our choice.",
            "why": "The wider search needed an evidence review before treating its ranking as settled.",
            "working": "We have a Texas paid-game example and a clearer alternative provider comparison.",
            "not_working": "No exact package has guaranteed Google approval; some research claims needed correction.",
            "next": "Use the corrected shortlist when you decide to form the LLC.",
        },
    )
    reviewed_body = '<p>The wider review found no demonstrated reason to replace Northwest for your planned low-volume start.</p>'
    reviewed_body += '<p><a href="https://play.google.com/store/apps/details?id=com.kice.afterplace">Afterplace</a> is a paid game by Evan Kice LLC, listed at the same street/suite as <a href="https://www.fmcsa.dot.gov/registration/process-agents/northwest-registered-agent-llc-55">Northwest’s Texas entry</a>.</p>'
    reviewed_body += '<p>That demonstrates the public Texas address outcome. The developer’s actual provider, package and private verification history remain unknown.</p>'
    reviewed_body += table(["Provider", "First year", "Annual renewal", "Mail and lease"], [
        ("Northwest, quoted bundle", "$438", "$224", "20 annual opened-and-scanned pieces; lease."),
        ('<a href="https://www.texasvirtualoffice.com/">Texas Virtual Office</a>, advertised bundle', "$615", "$205", "Unlimited scans; month-to-month lease."),
    ])
    reviewed_body += '<p>Both totals include formation, state filing, registered agent and address service. Unquoted extras and taxes remain excluded.</p>'
    reviewed_body += '<p>The alternative costs $177 more initially and $19 less each renewal year. Higher mail volume could favor it.</p>'
    reviewed_body += '<p>Its page also includes the first agent year. The report’s uncertainty about these advertised inclusions was incorrect.</p>'
    reviewed_body += '<p><a href="https://www.thefarmsoho.com/product/virtual-office/texas/">The Farm</a> offers workspace and document tiers at $218 or $350 monthly. No stronger approval outcome was demonstrated.</p>'
    reviewed_body += '<p>Two other corrections matter: shared software does not prove common ownership; <a href="https://www.northwestregisteredagent.com/terms-policy">Northwest’s terms</a> do not guarantee refunds before 90 days.</p>'
    reviewed_body += '<p>State fees are nonrefundable. The report’s proposed purchase test is not a risk-free trial.</p>'
    reviewed_body += '<p><a href="http://127.0.0.1:8776/?project=aquila-code-dark#1caac6482b7b4aa4803ae28d63b1c53d">Original research</a>; <a href="address-alternatives-checks.json">source checks and corrections</a>; <a href="publishing-reference.md">saved comparison</a>.</p>'
    reviewed_body += '<p>Read all 113 lines and preserved the original bytes. No purchase, signup or provider contact occurred.</p>'
    reviewed_alternatives += decide.info_thread("aquila-direction-r83-info",
        "Broader research supports Northwest, with a useful new alternative.",
        body_html=reviewed_body, short="Broader research reviewed", follows="aquila-direction-r82-info")
    reviewed_alternatives += decide.request("aquila-direction-r83-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Earlier game request remains pending for when you return to playtesting.",
        short="Earlier Windows feedback request", follows="aquila-direction-r82-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    reviewed_alternatives += decide.whats_next("aquila-direction-r83-next",
        "Use the corrected comparison when you decide to form the LLC.",
        summary="The research-run request is complete. No business action is required now.",
        short="Keep the corrected shortlist", follows="aquila-direction-r82-next")
    rounds = page.entry(83, "Research reviewed; Northwest still leads", reviewed_alternatives, when="2026-11-17") + rounds
    quality_priority = decide.recap(
        "Price and convenience influenced my Northwest recommendation too much. Its quote and app examples remain useful evidence. "
        "They do not prove superior reliability. Your priority is saved: robust, stable, proven service first; price second. "
        "Northwest remains a candidate; no quality winner established.",
        [
            ("Reason", "Price, combined services, the written quote and public app examples supported the earlier recommendation."),
            ("Correction", "I have not established that Northwest has the strongest service or track record."),
            ("Your priority", "Robustness, stability and proven service come before lower prices."),
            ("Evidence needed", "Compare address continuity, mail reliability, responsive support and sustained developer outcomes."),
            ("Status", "Preference saved; no provider chosen or purchase made."),
        ],
        {
            "did": "Saved your quality priority and corrected the current recommendation.",
            "why": "The earlier ranking gave startup cost and convenience too much weight for your needs.",
            "working": "The written quote and public app-address examples remain positive evidence.",
            "not_working": "Existing evidence does not establish a comparative reliability leader.",
            "next": "Use service quality and stability as the main criteria when comparing providers.",
        },
    )
    quality_body = '<p>Price was a significant factor, alongside combined services, the written quote and observed app-address examples.</p>'
    quality_body += '<p>I have not established that Northwest has the strongest track record or most dependable service.</p>'
    quality_body += table(["Earlier evidence", "What it supports"], [
        ('<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Combined services</a> and <a href="northwest-package-reply.json">written quote</a>', 'Convenience, scope and quoted cost; no measured reliability advantage.'),
        ('<a href="https://zensoul.app/">Zensoul</a> and <a href="https://play.google.com/store/apps/details?id=com.kice.afterplace">Afterplace</a>', 'Observed public-address outcomes; retain the provider/package qualifications in the saved comparison.'),
    ])
    quality_body += '<p>Your willingness to pay more is saved. No numeric budget inferred.</p>'
    quality_body += '<p>Future comparison should emphasize stable premises, reliable mail handling, responsive support and a demonstrated service record.</p>'
    quality_body += '<p>A higher price or longer feature list does not itself establish those qualities.</p>'
    quality_body += '<p>Northwest remains a candidate. The existing research does not establish a winner under these priorities.</p>'
    quality_body += '<p><a href="publishing-reference.md#quality-before-price-round-84">Saved priority and evidence gaps</a>. No new research job, provider contact or purchase occurred.</p>'
    quality_priority += decide.info_thread("aquila-direction-r84-info",
        "Yes, price influenced my recommendation; I weighted it too heavily.",
        body_html=quality_body, short="Quality before price", follows="aquila-direction-r83-info")
    quality_priority += decide.request("aquila-direction-r84-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Earlier game request remains pending for when you return to playtesting.",
        short="Earlier Windows feedback request", follows="aquila-direction-r83-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    quality_priority += decide.whats_next("aquila-direction-r84-next",
        "Use your quality priorities when the provider comparison resumes.",
        summary="No decision needed now. Comparative reliability evidence remains incomplete.",
        short="Compare dependable service", follows="aquila-direction-r83-next")
    rounds = page.entry(84, "Reliable service before lower prices", quality_priority, when="2026-11-17") + rounds
    quality_next = decide.recap(
        "Next, fill the reliability gap before choosing a provider. The focused research request is ready. "
        "It keeps our earlier evidence and compares stable addresses, mail handling, support and proven results. "
        "Run it in Perplexity, then save the full answer. We will use it to narrow the choice.",
        [
            ("Focus", "Compare dependable service and address stability before price."),
            ("Reuse", "Keep the existing provider reply, app examples and corrected research."),
            ("Shortlist", "Examine at most four finalists, including Northwest, without assuming a winner."),
            ("Decision", "Request a primary option, fallback and decisive questions for the strongest two providers."),
            ("Next action", "Run the prepared prompt in Perplexity and save its complete answer."),
        ],
        {
            "did": "Prepared a focused comparison of provider reliability and proven service.",
            "why": "The previous comparison did not establish who provides the most dependable service.",
            "working": "The saved prompt includes our existing evidence and matches the Console readback.",
            "not_working": "No research return or comparative quality winner yet.",
            "next": "You run the prompt; I review the evidence and narrow the choice.",
        },
    )
    quality_next_body = '<p>The next step targets missing service evidence. Existing findings remain useful.</p>'
    quality_next_body += table(["Step", "Purpose"], [
        ('Focused comparison', 'Check address continuity, reliable mail, responsive support and documented customer outcomes.'),
        ('Narrow the finalists', 'Separate formation and address services where that improves quality. Compare cost after fit and service.'),
        ('Resolve decisive gaps', 'Prepare precise questions for the strongest two providers before choosing an exact package.'),
    ])
    quality_next_body += '<p>The prompt asks what would justify choosing, so this does not become endless broad research.</p>'
    quality_next_body += '<p>It preserves Northwest’s reply, the positive app examples and corrections to the earlier report.</p>'
    quality_next_body += '<p><a href="http://127.0.0.1:8776/?project=aquila-code-dark#0013c8c0a3bb4ac982a44417de36ddf3">Complete research request</a>; <a href="address-reliability-receipt.json">submission receipt</a>; <a href="publishing-reference.md">saved next steps</a>.</p>'
    quality_next_body += '<p>Submitted prompt readback matches local bytes. Research Console stores the request; it does not execute Perplexity.</p>'
    quality_next_body += '<p>No provider contact, purchase or signup occurred.</p>'
    quality_next += decide.info_thread("aquila-direction-r85-info",
        "A focused reliability comparison is ready as the next step.",
        body_html=quality_next_body, short="Fill the service evidence gap", follows="aquila-direction-r84-info")
    quality_next += decide.request("aquila-direction-r85-research",
        "Run the focused comparison in Perplexity, then save its full answer in Research Console.",
        summary="This tests our shortlist against your quality priorities.",
        info='<p>Open <a href="http://127.0.0.1:8776/?project=aquila-code-dark#0013c8c0a3bb4ac982a44417de36ddf3">the prepared request</a>, copy its prompt, and run it. Import or paste the answer, then press Save result.</p>',
        short="Run the focused comparison",
        reply="Tell me when the complete result is saved.")
    quality_next += decide.request("aquila-direction-r85-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Earlier game request remains pending for when you return to playtesting.",
        short="Earlier Windows feedback request", follows="aquila-direction-r84-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    quality_next += decide.whats_next("aquila-direction-r85-next",
        "Review the returned evidence, then identify the best-supported route and remaining decisive questions.",
        summary="Waiting on the focused research answer. No provider selected.",
        short="Narrow the choice with evidence", follows="aquila-direction-r84-next")
    rounds = page.entry(85, "Next: compare dependable service", quality_next, when="2026-11-17") + rounds
    session_wrap = decide.recap(
        "Checkpoint and restart prompt saved. Native sources match the earlier tested versions; repository safety checks pass. "
        "The reliability research has returned but remains unread. Session-owned commit and push are in progress. "
        "The next session reads the checkpoint, reports status and waits.",
        [
            ("Saved", "Current decisions, limits and restart instructions are recorded."),
            ("Native", "Twenty-four hashes match prior evidence; input configuration differs only by one removed blank line."),
            ("Checks", "Six repository safety tests and the installed-engine check pass."),
            ("Research", "The focused result has arrived; its body remains unread and unreviewed."),
            ("Publication", "Preparing the authorized session-owned commit through an isolated index."),
        ],
        {
            "did": "Saved the wrap checkpoint and exact restart prompt.",
            "why": "You invoked the new-session workflow, including commit and push.",
            "working": "Source hashes and repository safety checks pass; the research return is identified.",
            "not_working": "Publication is not complete; research review and Windows gameplay feedback remain pending.",
            "next": "Commit and verify the push, then finish the restart handoff.",
        },
    )
    wrap_body = '<p>The wrap preserves native work, business decisions, research provenance and earlier report history.</p>'
    wrap_body += '<p>Native file contents match prior evidence except one removed blank line. No fresh runtime or phone testing is claimed.</p>'
    wrap_body += '<p>The focused research has returned as version 1. Only its metadata was inspected; the run request is complete.</p>'
    wrap_body += '<p>Reports and Research Console remain running. No session build, game or transfer remains active.</p>'
    wrap_body += '<p><a href="native-restart.txt">Restart prompt</a>; <a href="native-publication.json">publication evidence</a>; <a href="address-reliability-return.json">unreviewed return metadata</a>.</p>'
    wrap_body += '<p>Publication is in progress. The next session must read, report status and wait.</p>'
    session_wrap += decide.info_thread("aquila-direction-r86-info",
        "The checkpoint is saved; authorized publication is in progress.",
        body_html=wrap_body, short="Session wrap prepared", follows="aquila-direction-r85-info")
    session_wrap += decide.request("aquila-direction-r86-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Carried forward for a later playtest; no playtest starts during this wrap.",
        short="Earlier Windows feedback request", follows="aquila-direction-r85-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    session_wrap += decide.whats_next("aquila-direction-r86-next",
        "Verify publication, then hand over the saved restart prompt.",
        summary="Research review and game work wait for owner direction after the startup stop.",
        short="Finish publication and stop", follows="aquila-direction-r85-next")
    rounds = page.entry(86, "Session checkpoint saved", session_wrap, when="2026-11-17") + rounds
    published_native = decide.recap(
        "Session-owned native work and records are committed and pushed. Remote verification passed; shared staging was preserved. "
        "The restart prompt is saved. Reliability research has returned but remains unread. "
        "The next session reads the checkpoint, reports where things stand and waits.",
        [
            ("Published", "The content commit contains 213 session-owned paths and is present on the remote."),
            ("Preserved", "The isolated index protected shared staging; no dirty source remained after the content push."),
            ("Verified", "Git safety, repository tests and source-evidence checks pass; no fresh gameplay testing is claimed."),
            ("Pending", "Reliability research awaits review; Windows expedition feedback and later device checks remain open."),
            ("Handoff", "Restart instructions require reading, reporting status and waiting for your direction."),
        ],
        {
            "did": "Committed and pushed the native work, research records and saved decisions.",
            "why": "You invoked the complete new-session workflow.",
            "working": "Remote verification passed, shared staging was preserved, and the restart prompt is saved.",
            "not_working": "Research review, human expedition feedback, sustained device testing, cloud saves and Apple work remain unfinished.",
            "next": "Start a fresh session with the saved prompt, then choose the next task.",
        },
    )
    published_body = '<p>Native work, publishing decisions and research records are committed and pushed.</p>'
    published_body += '<p>Verified content commit: <code>64a1b619989a0d7c8674fa5014972b9b0abd23f0</code>.</p>'
    published_body += '<p>The remote branch matches that commit. The final records commit seals this receipt and handoff without embedding its own hash.</p>'
    published_body += '<p>Private-index publication preserved shared staging. Assets, builds, credentials and raw research remain outside Git.</p>'
    published_body += '<p>Six repository safety tests pass. Earlier native evidence remains applicable except one removed blank line in input configuration.</p>'
    published_body += '<p>Research version 1 has returned; its body remains unread and unreviewed. No repeat research run is needed.</p>'
    published_body += '<p><a href="native-restart.txt">Copy the restart prompt</a>; <a href="native-publication.json">publication evidence</a>; <a href="address-reliability-return.json">return metadata</a>.</p>'
    published_body += '<p>Reports and Research Console remain running. No game, provider, transfer or deployment work starts during this wrap.</p>'
    published_native += decide.info_thread("aquila-direction-r87-info",
        "Session-owned work is committed, pushed and verified remotely.",
        body_html=published_body, short="Publication verified", follows="aquila-direction-r86-info")
    published_native += decide.request("aquila-direction-r87-playtest",
        "After one Windows expedition, what felt confusing, slow, or unsatisfying, and did the upgrade survive reopening?",
        summary="Carried forward for later; the fresh session must first read, report status and wait.",
        short="Earlier Windows feedback request", follows="aquila-direction-r86-playtest",
        reply="What needs fixing, and did your progress survive reopening?")
    published_native += decide.whats_next("aquila-direction-r87-next",
        "Use the saved restart prompt; the next session reads, reports status and waits.",
        summary="Then choose research review or Windows expedition follow-up. No purchase, contact or deployment authority is added.",
        short="Ready for a fresh session", follows="aquila-direction-r86-next")
    rounds = page.entry(87, "Published and ready for restart", published_native, when="2026-11-17") + rounds
    reliability_review = decide.recap(
        "I’d check Northwest first. Four developers show the same Austin suite on Google Play. "
        "The research found no measured scan times or mail loss rates for these offices. "
        "Regus Plaza 7000 is the backup if its agreement gives you access you need.",
        [
            ("First choice to check", "Northwest combines a written quote, local mail handling duties and repeated public address examples."),
            ("Texas evidence", "Three more developers list the same Austin suite as Afterplace."),
            ("Known limit", "The listings do not identify the Texas packages or disclose verification paperwork."),
            ("Fallback", "Regus offers a staffed office at a different building for a higher price."),
            ("Still open", "Eligibility for your home-operated LLC depends on its exact address and permitted use."),
        ],
        {
            "did": "Read the complete return and checked the public listings, provider terms and Google’s requirements.",
            "why": "You put proven service ahead of price.",
            "working": "Four Austin listings show the public address outcome you want.",
            "not_working": "Comparable mail performance and eligibility for your exact package remain unknown.",
            "next": "Compare written terms at formation time. Five draft questions are saved.",
        },
    )
    review_body = table(["Provider", "What matters for your decision"], [
        ("Northwest", "Quoted $438 in year one and $224 annually afterward. Includes formation once, registered agent and 20 annual mail scans. "
         '<a href="northwest-package-reply.json">Written reply</a>. '
         '<a href="https://www.northwestregisteredagent.com/jobs/core-product/business-solutions-specialist-tx/">Austin mail duties</a>.'),
        ("Regus Plaza 7000", "Different staffed building. Texas records show a 2020–21 remodel. Mail performance and inspection permission remain unknown. "
         '<a href="https://www.regus.com/en/us/291">Center</a>. '
         '<a href="https://www.tdlr.texas.gov/TABS/Projects/TABS2020022346">Texas record</a>.'),
        ("Texas Virtual Office", "Lists another suite in the Balcones building. A building closure would affect both addresses. "
         '<a href="https://www.texasvirtualoffice.com/terms/">Address in its terms</a>.'),
        ("The Farm", "Premium plans advertise workspace and supporting documents. The review found no demonstrated Texas approval advantage. "
         '<a href="https://www.thefarmsoho.com/product/virtual-office/texas/">Austin offer</a>.'),
    ])
    review_body += (
        '<p>Afterplace, Offshore Inc., Skol Games and Pibardos show the same Austin street and suite. '
        'Federal records list two agents there. The listings do not identify these developers’ plans. '
        '<a href="https://play.google.com/store/apps/details?id=com.kice.afterplace">Afterplace</a>. '
        '<a href="https://play.google.com/store/apps/details?id=com.strongware.offshoreinc">Offshore Inc.</a> '
        '<a href="https://play.google.com/store/apps/details?id=com.skollabs.bible">Skol Games</a>. '
        '<a href="https://play.google.com/store/apps/details?id=com.filament3d">Pibardos</a>. '
        '<a href="https://www.fmcsa.dot.gov/registration/process-agents/northwest-registered-agent-llc-55">Northwest</a>. '
        '<a href="https://www.fmcsa.dot.gov/registration/process-agents/registered-agents-inc-88">Registered Agents Inc.</a></p>'
        '<p>Zensoul names Northwest in Wyoming. Its Play listing displays that address. '
        '<a href="https://zensoul.app/">Developer site</a>. '
        '<a href="https://play.google.com/store/apps/details?id=app.zensoul.app">Play listing</a>.</p>'
        '<p>Google publishes an organization’s legal address. Its payments details must match Dun &amp; Bradstreet. '
        'Northwest warned about Dun &amp; Bradstreet eligibility. Its reply reported no Google rejection. '
        '<a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google requirements</a>.</p>'
        '<p>Northwest quoted a unique number and a one-year lease document. Its website offers a month-to-month lease option. '
        'Exact street, contract duration and help during an office move still matter. '
        '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Texas plan</a>.</p>'
        '<p>Northwest’s terms stop forwarding after termination. Postal rules generally require six months of remailing unless customers give permitted written instructions. '
        'The mailbox agreement matters for ordinary mail. Legal notices need separate arrangements. '
        '<a href="https://www.northwestregisteredagent.com/terms-policy">Northwest terms</a>. '
        '<a href="https://pe.usps.com/text/dmm300/508.htm">Postal rule, section 1.8.4</a>.</p>'
        '<p>Davinci advertises the Regus location under a separate agreement. Its promotional figures total $2,691 initially, then $2,341 if rates hold. '
        'That includes self-filing, the state fee and a registered agent. A direct Regus quote remains open. '
        '<a href="https://www.davincivirtual.com/loc/us/texas/austin-virtual-offices/facility-4150">Reseller offer</a>. '
        '<a href="https://www.davincivirtual.com/terms-and-conditions">Reseller terms</a>.</p>'
        '<p><a href="publishing-reference.md">Saved publishing reference</a>. '
        '<a href="http://127.0.0.1:8776/?project=aquila-code-dark#0013c8c0a3bb4ac982a44417de36ddf3">Original return</a>.</p>'
    )
    review_body += table(["Provider", "Draft questions, unsent"], [
        ("Northwest", "Which street address, lease terms and permissions for public use would the $99 plan give our home-operated LLC?"),
        ("Northwest", "If the office moves or service ends, what notice, forwarding choices, fees and legal-notice arrangements apply?"),
        ("Northwest", "How do you track missing ordinary or certified mail, and what does each scan beyond 20 cost?"),
        ("Regus Plaza 7000", "Which local services and access rights would our exact address plan provide if Dun &amp; Bradstreet requested premises evidence?"),
        ("Regus Plaza 7000", "Could you confirm mailbox format, scanning fees, deposit, renewal costs and mail handling after a move?"),
    ])
    reliability_review += decide.info_thread(
        "aquila-direction-r88-info", "Northwest has the strongest evidence for checking its Texas address first.",
        body_html=review_body, short="Quality comparison reviewed", follows="aquila-direction-r87-info",
    )
    reliability_review += decide.whats_next(
        "aquila-direction-r88-next", "At formation time, check the street, permitted use and mail arrangements in writing.",
        summary="Consider Regus only if its agreement solves a specific gap. No provider selected or contacted.",
        short="Exact package still open", follows="aquila-direction-r87-next",
    )
    rounds = page.entry(88, "Northwest leads the address shortlist", reliability_review, when="2026-11-17") + rounds
    address_fit = decide.recap(
        "I put too much weight on the street number. What matters is which address players see. "
        "The mail question is whether you would see an important letter in time to act.",
        [
            ("Address", "This means the street and suite or mailbox number assigned to your LLC."),
            ("Privacy", "Google Play shows the legal address on your verified organization profile."),
            ("Mail", "A good service alerts you and scans every page when an important letter arrives."),
            ("Move", "Written notice and a forwarding plan help letters reach you through an office move."),
            ("Priority", "At your expected mail volume, a dependable scan matters more than unlimited scans."),
        ],
        {
            "did": "Explained which parts of the address and mail agreement matter for your business.",
            "why": "You want to keep your home private and still catch important mail.",
            "working": "Northwest already quoted a numbered unit, a lease and 20 opened-and-scanned items each year.",
            "not_working": "We still lack an enrollment outcome for your LLC or measured mail performance at the proposed office.",
            "next": "Use the privacy requirement first when judging the terms.",
        },
    )
    fit_body = table(["What matters", "Preferable", "Poor fit"], [
        ("Address players see",
         "Your LLC can document its assigned business address with Dun &amp; Bradstreet and Google.",
         "The service accepts your mail, but your verified public profile would still have to show your home."),
        ("A tax letter arrives",
         "You receive a prompt alert, a readable scan of every page and help tracing the original.",
         "You can see an envelope but cannot read the deadline inside or reach someone for help."),
        ("The office moves",
         "You get advance notice and know how letters addressed to the old office will reach you.",
         "The address changes and nobody explains where the mail goes."),
    ])
    fit_body += (
        '<p>Google takes the public legal address from your organization’s payments profile. It must match the Dun &amp; Bradstreet record. '
        'Giving residential ID privately to the mail provider is a separate step. '
        '<a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google’s public profile rules</a>. '
        '<a href="https://support.google.com/googleplay/android-developer/answer/13634888?hl=en">Address matching</a>.</p>'
        '<p>Afterplace shows one way a shared Austin suite can appear on Play. Another suitable business address would work equally well. '
        'A sample format is enough to compare plans before Northwest assigns your number. '
        '<a href="https://play.google.com/store/apps/details?id=com.kice.afterplace">Afterplace</a>.</p>'
        '<p>Northwest’s $99 annual offer includes 20 opened-and-scanned items and a lease. It advertises same-day digital scans. '
        'The quote describes a mailing address. Eligibility for your LLC’s public profile remains open. '
        '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Texas offer</a>. '
        '<a href="northwest-package-reply.json">Written reply</a>.</p>'
    )
    address_fit += decide.info_thread(
        "aquila-direction-r89-info", "A good fit protects your home address and gets important letters to you in time.",
        body_html=fit_body, short="What preferable means here", follows="aquila-direction-r88-info",
    )
    address_fit += decide.whats_next(
        "aquila-direction-r89-next", "Northwest remains the first provider I’d check when you compare agreements.",
        summary="No provider choice needed today.", short="Nothing needed now", follows="aquila-direction-r88-next",
    )
    rounds = page.entry(89, "Which address players see", address_fit, when="2026-11-17") + rounds
    privacy_first = decide.recap(
        "Keep your home address off public listings. Among plans that meet that requirement, dependable handling of important letters beats unlimited scans.",
        [
            ("Privacy", "Players should see a business address."),
            ("Mail", "An important letter should reach you in time to act."),
            ("Scans", "An unlimited allowance carries less weight."),
            ("Record", "Your priorities are saved for future comparisons."),
            ("Provider", "The package choice remains open."),
        ],
        {
            "did": "Saved the order you confirmed.",
            "why": "Home-address privacy comes first.",
            "working": "We know how you want qualifying plans compared.",
            "not_working": "The exact address and mail terms remain open.",
            "next": "Use your priorities when the provider decision returns.",
        },
    )
    privacy_first += decide.info_thread(
        "aquila-direction-r90-info", "Your provider priorities are saved.",
        body_html="<blockquote>privacy first, then favor dependable handling of important letters over unlimited scans.</blockquote>",
        short="Your confirmed order", follows="aquila-direction-r89-info",
    )
    privacy_first += decide.whats_next(
        "aquila-direction-r90-next", "Apply this order when comparing exact packages.",
        short="Provider choice open", follows="aquila-direction-r89-next",
    )
    rounds = page.entry(90, "Privacy comes first", privacy_first, when="2026-11-17") + rounds
    reply_timing = decide.recap(
        "I’d reply while the thread is fresh. You can choose a provider closer to formation. "
        "Northwest gave no deadline, and the referral request only applies if you sign up.",
        [
            ("Timing", "You already told Northwest that formation might be three to six months away."),
            ("Quote", "Their reply explains the price, lease and mail allowance."),
            ("Privacy", "Ask which office address the agreement lets your LLC display publicly."),
            ("Mail", "Ask what happens to letters sent to the old office after a move."),
            ("Decision", "Keep Northwest on the shortlist while the public address question remains open."),
        ],
        {
            "did": "Read their reply and drafted a short follow-up.",
            "why": "The remaining questions matter to your privacy and your mail.",
            "working": "You can ask those questions in the existing thread.",
            "not_working": "The exact package’s suitability for your public listing remains open.",
            "next": "Choose a provider when formation is closer.",
        },
    )
    email_draft = (
        "<p>Hi Northwest team,</p>"
        "<p>Thanks for explaining the pricing and what the lease covers. "
        "I’m still gathering information and may form the LLC in three to six months.</p>"
        "<p>Could you share the street address, city and ZIP for the $99 Texas office plan? "
        "Would your agreement allow my LLC to display that address on Google Play if Google accepts it?</p>"
        "<p>If the office moves, how much notice would I get? "
        "How would letters sent to the old address reach me?</p>"
        "<p>Thanks again for your help,<br>Jacob</p>"
    )
    reply_timing += decide.info_thread(
        "aquila-direction-r91-info", "I’d send a short follow-up. You can leave the purchase decision open.",
        body_html=email_draft, short="Suggested email reply", follows="aquila-direction-r90-info",
    )
    reply_timing += decide.whats_next(
        "aquila-direction-r91-next", "Reply when convenient. Their email gives no deadline.",
        summary="The draft is unsent.", short="Your timing", follows="aquila-direction-r90-next",
    )
    rounds = page.entry(91, "Replying to Northwest", reply_timing, when="2026-11-17") + rounds
    friendly_reply = decide.recap(
        "The revised reply thanks Northwest for explaining the renewal costs. A link to their mail policy would make answering easier.",
        [
            ("Opening", "Thank them for a detail that helped."),
            ("Timing", "Keep the three-to-six-month window tentative."),
            ("Address", "Ask for the office location and permission to display it."),
            ("Mail", "Ask how much notice customers get and what happens to letters after a move."),
            ("Reply", "Welcome a link to their usual policy."),
        ],
        {
            "did": "Rewrote the email in a warmer tone.",
            "why": "You want the representative to feel comfortable replying.",
            "working": "The draft is ready for the existing thread.",
            "not_working": "The address and mail questions remain open.",
            "next": "Reply whenever convenient.",
        },
    )
    friendly_draft = (
        "<p>Hi Northwest team,</p>"
        "<p>Thanks for walking me through the pricing and lease. "
        "It helped to see what I’d pay up front and what renews each year. "
        "I’m still planning and may form the LLC in three to six months.</p>"
        "<p>When you have a chance, could you share the street address, city and ZIP for the $99 Texas office plan? "
        "I understand the unit number comes later. "
        "If Google accepts it, would your terms allow my LLC to display that address publicly on Google Play?</p>"
        "<p>If the office ever moves, how much notice would I get? "
        "How would letters sent to the old address reach me? "
        "A link to your mail policy would be great if that’s easier.</p>"
        "<p>Thanks again for helping me think this through!<br>Jacob</p>"
    )
    friendly_reply += decide.info_thread(
        "aquila-direction-r92-info", "A warmer reply for the existing email thread.",
        body_html=friendly_draft, short="Revised email", follows="aquila-direction-r91-info",
    )
    friendly_reply += decide.whats_next(
        "aquila-direction-r92-next", "The draft is ready to send when convenient.",
        short="On your schedule", follows="aquila-direction-r91-next",
    )
    rounds = page.entry(92, "A friendlier reply to Northwest", friendly_reply, when="2026-11-17") + rounds
    site_check = decide.recap(
        "I checked the plan and terms earlier. I missed the office directory and the Albany move article. "
        "The email can focus on what happens to Texas customers’ mail if the office moves.",
        [
            ("City", "Northwest lists Austin, Texas 78731."),
            ("Street", "They explain why they give customers the full address after signup."),
            ("Public use", "Northwest allows the address on websites and other public business materials."),
            ("Past move", "They promised three months of mail collection after their Albany registered office moved in 2022."),
            ("Texas", "I found no written notice or collection period for a Texas virtual-office move."),
        ],
        {
            "did": "Checked each question in the draft against Northwest’s public pages.",
            "why": "Some details were already available.",
            "working": "Their directory and service description answer the general location and public use questions.",
            "not_working": "The Albany example leaves Texas customers’ terms open. Google Play eligibility also remains open.",
            "next": "Ask only about advance notice and ordinary mail after an Austin office move.",
        },
    )
    check_body = table(["Email topic", "What the website says"], [
        ("Address", 'The directory lists Austin, TX 78731. Northwest withholds street addresses from its directory until signup. '
         '<a href="https://www.northwestregisteredagent.com/office-locations">Office directory</a>.'),
        ("Public display", 'The virtual office page allows websites, social media and other public business materials. '
         'It makes no specific promise about Google Play verification. '
         '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office">Virtual office uses</a>.'),
        ("An office move", 'Northwest described three months of collection at the old Albany address. '
         'I found no Texas notice period or promise covering ordinary mail after a move. '
         '<a href="https://www.northwestregisteredagent.com/registered-agent/address-change">Albany move</a>. '
         '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Texas plan</a>.'),
    ])
    check_body += (
        "<p>Hi Northwest team,</p>"
        "<p>Thanks for explaining the lease and renewal costs. I’m still in the planning stage, and your breakdown helped.</p>"
        "<p>I saw your note about collecting mail for three months after your Albany office moved. "
        "Would that also apply to ordinary business mail on the $99 Texas virtual office plan if the Austin office moved? "
        "How much advance notice would customers usually get?</p>"
        "<p>Thanks again for your help,<br>Jacob</p>"
    )
    site_check += decide.info_thread(
        "aquila-direction-r93-info", "Some answers were on the site. I narrowed the draft.",
        body_html=check_body, short="Sources and shorter email", follows="aquila-direction-r92-info",
    )
    site_check += decide.whats_next(
        "aquila-direction-r93-next", "The shorter draft is ready for the existing email thread.",
        summary="Nothing sent.", short="Reply when convenient", follows="aquila-direction-r92-next",
    )
    rounds = page.entry(93, "What Northwest already answers", site_check, when="2026-11-17") + rounds
    remaining = decide.recap(
        "Yes. The main question is whether players can see Northwest’s address while your home address stays private. "
        "The mail question is how their Austin team would trace a delivered letter when its scan is missing.",
        [
            ("Privacy", "Google Play publishes the organization’s verified legal address."),
            ("Verification", "The address in Google payments must match the LLC’s Dun & Bradstreet record."),
            ("Examples", "Four developers display the Austin suite. Their listings give no details about their plans or verification."),
            ("Mail", "Northwest advertises same-day scans. I found no response time for a missing scan on this plan."),
            ("Timing", "You can leave the email thread there for now."),
        ],
        {
            "did": "Checked what still matters assuming the office stays put.",
            "why": "Your first concern is which address players see.",
            "working": "Northwest has explained its price, lease and routine mail service.",
            "not_working": "Neither Google nor Dun & Bradstreet has verified an address for your future LLC.",
            "next": "Check which address the LLC can document before paying for this plan for privacy.",
        },
    )
    remaining += decide.info_thread(
        "aquila-direction-r94-info", "Two questions affect the choice once a move is set aside.",
        body_html=table(["Question", "What we know"], [
            ("Can your LLC use the Austin address on Google Play?",
             'Google publishes the legal address from your linked payments profile. It must match the organization’s Dun &amp; Bradstreet record. '
             'Northwest warned about virtual offices. It reported no Google rejection. '
             '<a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google’s display rules</a>. '
             '<a href="https://support.google.com/googleplay/android-developer/answer/13634888?hl=en">Address matching</a>. '
             '<p>Afterplace and three other developers display the suite listed for Northwest. '
             'Their listings do not identify the plan or documents they used. '
             '<a href="https://play.google.com/store/apps/details?id=com.kice.afterplace">Afterplace</a>. '
             '<a href="https://www.fmcsa.dot.gov/registration/process-agents/northwest-registered-agent-llc-55">Northwest’s federal listing</a>.</p>'),
            ("Who traces a letter marked delivered if no scan appears?",
             'The $99 Texas plan includes 20 scans per year and advertises same-day notification. '
             'I found no promised tracing time or price for scan 21 on that plan. '
             '<a href="https://www.northwestregisteredagent.com/business-address/virtual-office/texas">Texas mail terms</a>.'),
        ]),
        short="Privacy and ordinary mail", follows="aquila-direction-r93-info",
    )
    remaining += decide.whats_next(
        "aquila-direction-r94-next", "No reply needed now. Revisit the address evidence when formation is closer.",
        summary="Northwest remains the leading candidate.", short="Before choosing", follows="aquila-direction-r93-next",
    )
    rounds = page.entry(94, "If the Austin office stays put", remaining, when="2026-11-17") + rounds
    confirm_address = decide.recap(
        "I meant who could find a delivered letter whose scan never appeared. We can assume same-day scans for planning. "
        "For privacy, ask Dun & Bradstreet which address it would verify. Google later shows the address on the verified profile.",
        [
            ("Mail", "Drop the missing-letter question from the email."),
            ("Before paying", "Dun & Bradstreet’s Google developer page has a contact form and a Call link."),
            ("Address", "Explain that you work from home and would use Northwest for its mailing address and lease."),
            ("Evidence", "Ask which address would appear on your business record and what documents would support it."),
            ("Before publishing", "Check the Play developer profile after Google verifies your organization."),
        ],
        {
            "did": "Checked the D&B contact route and Google’s rules for US organizations.",
            "why": "You want players to see the business address while keeping your home address private.",
            "working": "You can ask for written guidance before paying Northwest.",
            "not_working": "The future LLC has no verified address yet.",
            "next": "Start with the address question for D&B when you want to pursue it.",
        },
    )
    address_steps = table(["When", "How to check"], [
        ("Before purchase",
         'Dun &amp; Bradstreet’s page has a <strong>Let’s get in touch</strong> form and a <strong>Call</strong> link. '
         'Ask for written guidance about a home-based Texas LLC. '
         '<a href="https://www.dnb.com/en-us/smb/duns/google-developers.html">D&amp;B’s Google developer page</a>.'),
        ("If D&B requires your home address",
         'If that is the address Google must match, Northwest’s mailing service alone cannot keep it off Play. '
         '<a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google’s organization address rules</a>.'),
        ("After formation",
         'Check the LLC’s legal name and full business address on its validated D&amp;B record. '
         'Match them in Google payments and complete organization verification. '
         'Google lists IRS-issued letters among the US documents it accepts. '
         '<a href="https://support.google.com/googleplay/android-developer/answer/15633622?hl=en">US verification documents</a>.'),
        ("Before publishing",
         'After verification, open <strong>Manage account &gt; Account details &gt; Play developer profile</strong>. '
         'Check that the address shown there is the Northwest address. '
         '<a href="https://support.google.com/googleplay/android-developer/answer/13634081?hl=en">Google’s profile instructions</a>.'),
    ])
    address_steps += (
        "<blockquote><p>Hi, I’m planning a Texas LLC that I’ll run from home. "
        "Northwest would provide a virtual-office mailing address and a lease. I wouldn’t work at that office.</p>"
        "<p>Could the Northwest address be the business address on the D-U-N-S record Google Play checks? "
        "Or could it only be a mailing address? What documents would you need? Thanks for your help.</p></blockquote>"
    )
    confirm_address += decide.info_thread(
        "aquila-direction-r95-info", "D&B can explain its address policy before the LLC exists.",
        body_html=address_steps, short="The address question and proof", follows="aquila-direction-r94-info",
    )
    confirm_address += decide.whats_next(
        "aquila-direction-r95-next", "The question is ready if you want D&B’s guidance. Nothing has been sent.",
        summary="No Northwest follow-up needed now.", short="Before spending", follows="aquila-direction-r94-next",
    )
    rounds = page.entry(95, "How to confirm the public address", confirm_address, when="2026-11-17") + rounds
    duns_inquiry = decide.recap(
        "Yes. Your LLC needs a D-U-N-S number to create its Google Play organization account. "
        "You can ask D&B about the address now. The standard application is free.",
        [
            ("Purpose", "Google uses this nine-digit business ID to verify the organization."),
            ("Today", "You can ask the address question before forming the LLC."),
            ("After formation", "Look up the LLC first. Request a number if it has none."),
            ("Application", "Have the legal name, business address, phone and owner’s name ready."),
            ("Timing", "Dun & Bradstreet says standard processing can take up to 30 business days."),
        ],
        {
            "did": "Checked the application details and rewrote the message for D&B.",
            "why": "You wanted to know when the number matters and what to gather.",
            "working": "Texas formation and the address inquiry can both happen before you have a number.",
            "not_working": "The address D&B would verify for your future LLC remains open.",
            "next": "Use the draft when you want address guidance.",
        },
    )
    duns_body = table(["When", "What you need"], [
        ("Planning the LLC",
         'The Texas formation filing asks for no D-U-N-S number. You can also ask D&amp;B about the address now. '
         '<a href="https://www.sos.state.tx.us/corp/instructions/205.shtml">Texas LLC filing instructions</a>.'),
        ("Getting a number",
         'After formation, look up the LLC. If needed, choose the Google developer application. '
         'D&amp;B may request the legal name, address, business phone and owner’s name. '
         'Other details include legal structure, founding year, industry and employee count. '
         'Standard processing is free and can take up to 30 business days. '
         '<a href="https://www.dnb.com/en-us/smb/duns/get-a-duns.html">D&amp;B’s application guide</a>.'),
        ("Opening a Google Play organization account",
         'Google requires the number and checks the LLC’s name and address against D&amp;B. '
         '<a href="https://support.google.com/googleplay/android-developer/answer/13628312?hl=en">Google Play requirements</a>.'),
    ])
    duns_body += (
        '<p><a href="https://www.dnb.com/en-us/smb/duns/google-developers.html">Contact D&amp;B about the address</a>.</p>'
        "<blockquote><p>Hi D&amp;B team,</p>"
        "<p>I’m planning a small game studio that I’ll run from my home in Texas. "
        "I’d appreciate your advice before I form the LLC and choose an address service.</p>"
        "<p>I’d like to keep my home address off Google Play. "
        "I’m considering Northwest Registered Agent’s virtual office, which comes with a mailing address and a lease. "
        "Could that address appear as my LLC’s business address on the D-U-N-S record Google Play checks? "
        "Or would it only be a mailing address? If it can qualify, what paperwork should I have ready?</p>"
        "<p>Thanks for helping me get this right!<br>Jacob</p></blockquote>"
    )
    duns_inquiry += decide.info_thread(
        "aquila-direction-r96-info", "A friendlier inquiry and what the number means for your LLC.",
        body_html=duns_body, short="Draft and application details", follows="aquila-direction-r95-info",
    )
    duns_inquiry += decide.whats_next(
        "aquila-direction-r96-next", "The draft is ready. The number can wait until after formation.",
        summary="Nothing submitted.", short="On your schedule", follows="aquila-direction-r95-next",
    )
    rounds = page.entry(96, "When your LLC needs a D-U-N-S number", duns_inquiry, when="2026-11-17") + rounds
    chat_receipt = ROOT / "Docs/Research/dnb-google-play-address-chat-v1.json"
    resume_message = json.loads(chat_receipt.read_text(encoding="utf-8"))["agent_suggested_message"]
    dnb_chat = decide.recap(
        "D&B’s chat couldn’t forward an email. Nobody was available for live text chat that evening. "
        "Tomorrow, paste the saved message and ask for a live representative.",
        [
            ("Email", "The bot acknowledged the approved name and email, then said it could not forward the question."),
            ("Live text", "It said a D&B representative can join this same chat when someone is available."),
            ("Hours", "The bot could not give reliable staffing hours."),
            ("Address", "No specialist has confirmed whether the Northwest address can be primary or what documents would qualify."),
            ("Tomorrow", "The bot supplied a short message in case a new chat opens."),
        ],
        {
            "did": "Read the full chat and saved the bot’s suggested message.",
            "why": "You want to pick up tomorrow without repeating the conversation.",
            "working": "The summary explains that the studio would operate from home in Texas.",
            "not_working": "The AI could not give an address decision or create an email referral.",
            "next": "Return November 18 and ask for a live person in text chat.",
        },
    )
    chat_body = (
        '<p>Paste this if a new chat starts.</p>'
        f'<blockquote><p>{escape(resume_message)}</p></blockquote>'
        '<p>Then ask, “Could a live representative join this chat?”</p>'
        '<p><a href="https://www.dnb.com/en-us/smb/duns/google-developers.html">Open D&amp;B’s Google developer page</a> · '
        '<a href="dnb-address-chat.json">Chat receipt and saved wording</a></p>'
    )
    dnb_chat += decide.info_thread(
        "aquila-direction-r97-info", "D&B’s exact wording for tomorrow.", body_html=chat_body,
        short="Message to paste", follows="aquila-direction-r96-info",
    )
    dnb_chat += decide.whats_next(
        "aquila-direction-r97-next", "Retry live text chat when you return November 18.",
        summary="No email referral to wait for.", short="Tomorrow’s chat", follows="aquila-direction-r96-next",
    )
    rounds = page.entry(97, "Resume D&B live chat tomorrow", dnb_chat, when="2026-11-17") + rounds
    browser_cleanup = decide.recap(
        "I stopped the helper I used for D&B. The tab had already closed, but the helper was still waiting for input.",
        [
            ("Purpose", "The helper controlled the separate Chrome window used for D&B chat."),
            ("Earlier reply", "I identified the task and left it running."),
            ("Window", "When I checked again, that browser had no open tabs."),
            ("Shutdown", "The terminal exited cleanly. I verified that Windows had released both processes."),
            ("Chat question", "The exact wording for a live representative remains saved."),
        ],
        {
            "did": "Stopped the idle D&B browser helper.",
            "why": "The chat tab had already closed.",
            "working": "The saved question is ready for a fresh chat.",
            "not_working": "D&B has yet to confirm which address qualifies.",
            "next": "The live-chat retry can wait until you want it.",
        },
    )
    browser_cleanup += decide.info_thread(
        "aquila-direction-r98-info", "The background task was the browser helper from the D&B conversation.",
        body_html=('<p>The terminal exited with status zero. Windows showed both the helper and its Chrome process had ended.</p>'
                   '<p><a href="dnb-address-chat.json">Saved D&amp;B question</a></p>'),
        short="What was running", follows="aquila-direction-r97-info",
    )
    browser_cleanup += decide.whats_next(
        "aquila-direction-r98-next", "Use the saved question when you return to D&B.",
        summary="Nothing needed for cleanup.", short="Saved wording", follows="aquila-direction-r97-next",
    )
    rounds = page.entry(98, "D&B browser helper stopped", browser_cleanup, when="2026-11-18") + rounds
    current_status = decide.recap(
        "Northwest is the leading candidate. D&B has yet to confirm whether the Austin office qualifies as the LLC’s primary address. "
        "The native expedition is built and pushed. A Windows playtest with a person is still pending.",
        [
            ("Privacy", "The owner wants to keep their home address off Google Play."),
            ("Austin office", "Four developers list the suite on Google Play. Their listings do not show how they verified it."),
            ("D&B chat", "The bot could not send an email referral. No live representative was available last night."),
            ("Background task", "The D&B browser helper has stopped."),
            ("Game testing", "Eleven automation groups passed. Windows and Android packages built successfully."),
        ],
        {
            "did": "Saved the exact question for a live D&B representative.",
            "why": "The studio would work from home and receive mail at Northwest’s office.",
            "working": "A fresh chat can pick up the address question without repeating last night’s conversation.",
            "not_working": "Which documents D&B would accept for this address remains open.",
            "next": "Retry live text chat before choosing an address package.",
        },
    )
    current_status += decide.info_thread(
        "aquila-direction-r99-info", "The saved message asks D&B which address and documents would qualify.",
        body_html=chat_body, short="Question for D&B", follows="aquila-direction-r98-info",
    )
    current_status += decide.whats_next(
        "aquila-direction-r99-next", "On the next visit, paste the saved question and ask for a live representative.",
        summary="No provider decision needed today.", short="Live chat", follows="aquila-direction-r98-next",
    )
    rounds = page.entry(99, "Northwest address and Windows playtest", current_status, when="2026-11-18") + rounds
    paused_follow_up = decide.recap(
        "The D&B follow-up is saved and paused. The full chat and the exact message are ready whenever you return.",
        [
            ("Decision", "The November 18 retry was deferred. There is no new date."),
            ("Chat", "The full conversation is saved locally with the contact details removed."),
            ("Referral", "The bot could not forward an email. No specialist answered."),
            ("Address", "Northwest leads. D&B has yet to confirm whether its Texas office qualifies as the primary address."),
            ("Restart", "The saved message asks which documents D&B accepts for a studio operated from home."),
        ],
        {
            "did": "Saved the chat, exact message and restart prompt.",
            "why": "You want to work on other things before returning to this question.",
            "working": "The saved transcript matches the original redacted capture.",
            "not_working": "There is no case number or human answer to follow up on.",
            "next": "Leave the representative follow-up paused until you ask to resume.",
        },
    )
    resume_later = (
        '<p>After you reopen this topic, ask for a live representative and paste the bot’s suggested message.</p>'
        f'<blockquote><p>{escape(resume_message)}</p></blockquote>'
        '<p><a href="https://www.dnb.com/en-us/smb/duns/google-developers.html">D&amp;B’s Google developer page</a> · '
        '<a href="dnb-address-chat.json">Chat receipt and local transcript location</a> · '
        '<a href="dnb-follow-up-restart.txt" download>Download the restart prompt</a></p>'
    )
    paused_follow_up += decide.info_thread(
        "aquila-direction-r100-info", "Where to pick up the representative conversation.",
        body_html=resume_later, short="Resume later", follows="aquila-direction-r99-info",
    )
    paused_follow_up += decide.whats_next(
        "aquila-direction-r100-next", "When you want to return, say “Resume the D&B representative question.”",
        summary="No decision needed for this save.", short="Follow-up paused", follows="aquila-direction-r99-next",
    )
    rounds = page.entry(100, "D&B follow-up saved for later", paused_follow_up, when="2026-11-18") + rounds
    camera_retest = decide.recap(
        "The rebuilt Windows game saved your signal investigation and closed normally. Hold either mouse button over the ship to look around.",
        [
            ("Mouse", "Fast drags now turn the camera. Either button works over open space."),
            ("Release", "The exterior view holds briefly, then eases behind the ship. The bridge keeps its heading."),
            ("Pause", "You can look around while the expedition clock stays paused."),
            ("Game checks", "The packaged game turned 90 degrees in both views during the same simulated drag."),
            ("Controls", "The expedition button grew from 15 to 30 pixels inside its border at 960 by 540."),
        ],
        {
            "did": "Rebuilt the desktop camera and made the commands larger in small windows.",
            "why": "You could barely turn the camera during your first playtest.",
            "working": "Both camera views passed. All eleven simulation and save checks also passed.",
            "not_working": "Turning comfort with your mouse still needs your verdict.",
            "next": "Try one sideways drag from the ship or the stars.",
        },
    )
    camera_body = '<p>The old camera discarded fast movements. The new camera reads each movement while you hold either button.</p>'
    camera_body += '<p>A simulated drag turned the actual game camera through a quarter turn, including while paused.</p>'
    camera_body += '<p>Your playtest save reached the travel phase. The window closed normally after saving revision 25.</p>'
    camera_body += '<p><a href="windows-camera-playtest.json">Camera checks and button measurements</a></p>'
    camera_body += page.figure(page.copy_media(HERE / "media/r101-desktop-before-960.png", out),
        "101.1 Before.", "Earlier Windows build at 960 by 540. The expedition button had 15 pixels of fill vertically.")
    camera_body += page.figure(page.copy_media(HERE / "media/r101-desktop-after-960.png", out),
        "101.2 After.", "Same window size. The button has 30 pixels of fill and the mouse instructions appear below.")
    camera_retest += decide.info_thread("aquila-direction-r101-info",
        "Fast mouse drags now turn the desktop camera.", body_html=camera_body,
        short="Windows camera fix", follows="aquila-direction-r74-info")
    camera_retest += decide.question("aquila-direction-r101-camera",
        "When you hold either mouse button over the ship and drag sideways, how does turning feel?",
        [("comfortable", "Comfortable now"), ("slow", "Still too slow"), ("fast", "Too fast")],
        short="Camera feel", follows="aquila-direction-r74-next")
    camera_retest += decide.whats_next("aquila-direction-r101-next",
        "Use your feedback to tune the camera or continue the expedition playtest.",
        summary="The separate Windows save is ready to resume.", short="Windows playtest")
    rounds = page.entry(101, "Windows camera ready to try", camera_retest, when="2026-11-18") + rounds
    desktop_controls = decide.recap(
        "Hold the right mouse button over the ship and drag sideways to see its side. "
        "Left-click the controls to give orders. The desktop controls are smaller.",
        [
            ("Orbit", "Hold either mouse button over the ship or stars. Drag sideways to orbit or vertically to tilt."),
            ("Release", "The pointer comes back. Outside, the camera eases behind the ship. On the bridge, it keeps your angle."),
            ("Orders", "Left-click a command and the helm steers the ship."),
            ("Desktop", "At 1280 by 720, the bottom panel shrank from 1,204 to 854 pixels wide."),
            ("Phone", "All three top touch buttons kept their previous dimensions in the Windows phone preview."),
        ],
        {
            "did": "Shrank the desktop controls and added a cue when the mouse grabs the view.",
            "why": "The last desktop change made the controls too big for you.",
            "working": "Both views turned during simulated mouse drags. A command click worked after release.",
            "not_working": "How the physical mouse feels to you is still open.",
            "next": "Look for “Orbiting” when you hold the right button over the ship.",
        },
    )
    controls_body = '<p>While you hold a mouse button over space, the pointer hides and the hint reads “Orbiting.”</p>'
    controls_body += '<p>On the bridge it reads “Looking around.” You can walk there with W, A, S and D.</p>'
    controls_body += '<p>The packaged game kept the drag when the pointer crossed a button. Releasing made the buttons clickable again.</p>'
    controls_body += '<p><a href="windows-desktop-controls.json">Button measurements and mouse checks</a></p>'
    controls_body += page.figure(page.copy_media(HERE / "media/r102-desktop-before-1280.png", out),
        "102.1 Previous desktop size.", "The bottom commands covered most of the window width.")
    controls_body += page.figure(page.copy_media(HERE / "media/r102-desktop-after-1280.png", out),
        "102.2 Smaller desktop controls.", "Same window size. The narrower strip leaves open space around the ship.", look=True)
    controls_body += page.figure(page.copy_media(HERE / "media/r102-touch-layout.png", out),
        "102.3 Touch layout.", "Windows preview of the large phone buttons. No new Galaxy playtest this round.")
    desktop_controls += decide.info_thread("aquila-direction-r102-info",
        "The Windows command strip now takes two-thirds of the play window.", body_html=controls_body,
        short="Smaller desktop controls", follows="aquila-direction-r101-info")
    desktop_controls += decide.question("aquila-direction-r102-camera",
        "When you hold the right mouse button over the ship and drag sideways, can you see its side?",
        [("turns", "Yes, it turns"), ("barely", "It barely moves"), ("still", "Nothing moves")],
        short="Check the orbit", follows="aquila-direction-r101-camera")
    desktop_controls += decide.whats_next("aquila-direction-r102-next",
        "Use your mouse result to decide whether the camera needs another fix.",
        summary="Your separate expedition save is ready for the check.", short="Windows mouse check",
        follows="aquila-direction-r101-next")
    rounds = page.entry(102, "Mouse controls and a smaller desktop panel", desktop_controls, when="2026-11-18") + rounds
    orbit_accepted = decide.recap(
        "You confirmed the mouse rotates the view around the ship. Your first expedition is saved with the helm on Hold.",
        [
            ("Mouse", "You could see the side of the ship when you dragged."),
            ("Build", "You tried the Windows build with the smaller controls."),
            ("Save", "The game saved revision 37 when the window closed."),
            ("Helm", "Your ship is holding position during the first signal investigation."),
            ("Pacing", "A full expedition playthrough remains open."),
        ],
        {
            "did": "Recorded your confirmation of the exterior camera.",
            "why": "You had reported that the camera barely turned.",
            "working": "Dragging with your mouse now turns the view around the ship.",
            "not_working": "Full expedition pacing still needs a playthrough.",
            "next": "Choose Resume course next time to head toward the signal.",
        },
    )
    orbit_accepted += decide.info_thread("aquila-direction-r103-info",
        "You confirmed the exterior camera works with your mouse.",
        body_html='<blockquote>Yeah, works good.</blockquote><p><a href="windows-orbit-acceptance.json">Playtest confirmation and save details</a></p>',
        short="Mouse check complete", follows="aquila-direction-r102-camera")
    orbit_accepted += decide.whats_next("aquila-direction-r103-next",
        "Choose Resume course when you next play to continue toward the signal.",
        short="Continue the expedition", follows="aquila-direction-r102-next")
    rounds = page.entry(103, "Windows mouse orbit confirmed", orbit_accepted, when="2026-11-18") + rounds
    (out / "windows-orbit-acceptance.json").write_bytes((ROOT / "Docs/Testing/native-windows-orbit-acceptance.json").read_bytes())
    (out / "windows-desktop-controls.json").write_bytes((ROOT / "Docs/Testing/native-windows-desktop-controls.json").read_bytes())
    (out / "windows-camera-playtest.json").write_bytes((ROOT / "Docs/Testing/native-windows-camera-playtest.json").read_bytes())
    (out / "dnb-follow-up-restart.txt").write_bytes((ROOT / "Docs/Handoffs/PROMPT_2026-11-18_dnb-follow-up.txt").read_bytes())
    (out / "dnb-address-chat.json").write_bytes(chat_receipt.read_bytes())
    (out / "native-restart.txt").write_bytes((ROOT / "Docs/Handoffs/PROMPT_2026-11-17_native-direction.txt").read_bytes())
    (out / "native-publication.json").write_bytes((ROOT / "Docs/Testing/native-direction-publication.json").read_bytes())
    (out / "address-reliability-return.json").write_bytes((ROOT / "Docs/Research/texas-business-address-reliability-return-v0001.json").read_bytes())
    (out / "address-reliability-receipt.json").write_bytes((ROOT / "Docs/Research/texas-business-address-reliability-receipt-v1.json").read_bytes())
    (out / "address-alternatives-checks.json").write_bytes((ROOT / "Docs/Research/texas-llc-address-alternatives-source-checks-v0001.json").read_bytes())
    (out / "address-alternatives-receipt.json").write_bytes((ROOT / "Docs/Research/texas-llc-address-alternatives-receipt-v1.json").read_bytes())
    (out / "northwest-package-reply.json").write_bytes((ROOT / "Docs/Research/northwest-texas-package-reply-v1.json").read_bytes())
    (out / "expedition-verification.json").write_bytes((ROOT / "Docs/Testing/native-expedition-verification.json").read_bytes())
    (out / "northwest-inquiry-confirmation.json").write_bytes((ROOT / "Docs/Research/northwest-inquiry-confirmation-v1.json").read_bytes())
    (out / "northwest-inquiry-v4.txt").write_bytes((ROOT / "Docs/Research/northwest-texas-package-inquiry-v4.txt").read_bytes())
    (out / "northwest-inquiry-v3.txt").write_bytes((ROOT / "Docs/Research/northwest-texas-package-inquiry-v3.txt").read_bytes())
    (out / "northwest-inquiry-v2.txt").write_bytes((ROOT / "Docs/Research/northwest-texas-package-inquiry-v2.txt").read_bytes())
    (out / "northwest-inquiry.txt").write_bytes((ROOT / "Docs/Research/northwest-texas-package-inquiry-v1.txt").read_bytes())
    (out / "northwest-package-check.json").write_bytes((ROOT / "Docs/Research/northwest-texas-package-check-v1.json").read_bytes())
    (out / "publishing-reference.md").write_bytes((ROOT / "knowledge/publishing-business.md").read_bytes())
    (out / "mobile-ui-verification.json").write_bytes((ROOT / "Docs/Testing/native-mobile-ui-verification.json").read_bytes())
    (out / "native-verification.json").write_bytes((ROOT / "Docs/Testing/native-foundation-verification.json").read_bytes())
    (out / "android-device-verification.json").write_bytes((ROOT / "Docs/Testing/native-android-device-verification.json").read_bytes())
    for source, name in [
        (ROOT / "Docs/History/game-direction/20261115T225503Z.md", "handoff.md"),
        (ROOT / "Docs/Handoffs/PROMPT_2026-11-15_game-direction.txt", "restart.txt"),
        (ROOT / "Docs/Testing/game-direction-publication.json", "publication.json"),
    ]:
        (out / name).write_bytes(source.read_bytes())
    doc = page.document("Aquila game and website direction", "<main>" + rounds + "</main>", "aquila-game-direction", folder=HERE)
    decide.require_ok(doc, root=out)
    decide.save_questions(doc, folder=HERE)
    (out / "index.html").write_text(doc, encoding="utf-8", newline="\n")
    print(out / "index.html")


if __name__ == "__main__":
    main(sys.argv[1])
