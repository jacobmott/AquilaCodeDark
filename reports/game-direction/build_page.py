"""Local Aquila direction and implementation report; prior rounds remain intact."""
import importlib.util
import sys
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
    doc = page.document("Aquila game and website direction", "<main>" + rounds + "</main>", "aquila-game-direction", folder=HERE)
    decide.require_ok(doc, root=out)
    decide.save_questions(doc, folder=HERE)
    (out / "index.html").write_text(doc, encoding="utf-8", newline="\n")
    print(out / "index.html")


if __name__ == "__main__":
    main(sys.argv[1])
