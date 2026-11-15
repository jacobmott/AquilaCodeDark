import {
  AfterViewInit,
  ChangeDetectorRef,
  Component,
  ElementRef,
  HostListener,
  NgZone,
  OnDestroy,
  ViewChild,
  isDevMode,
} from '@angular/core';
import { CommonModule } from '@angular/common';
import {
  NavigationEnd,
  Router,
  RouterLink,
  RouterOutlet,
} from '@angular/router';
import { Subscription, filter } from 'rxjs';
import {
  FLEET,
  FLAGSHIP,
  HOME_WORLD,
  Preferences,
  Quality,
  readFlightSave,
  readPreferences,
  Vessel,
  View,
} from './universe-model';
import { FlightState, UniverseScene } from './universe-scene';

@Component({
  selector: 'app-universe',
  standalone: true,
  imports: [CommonModule, RouterLink, RouterOutlet],
  templateUrl: './universe.component.html',
  styleUrl: './universe.component.css',
})
export class UniverseComponent implements AfterViewInit, OnDestroy {
  @ViewChild('sceneHost', { static: true }) host!: ElementRef<HTMLElement>;
  readonly fleet = FLEET;
  readonly qualityOptions: { value: Quality; label: string }[] = [
    { value: 'low', label: 'Light' },
    { value: 'balanced', label: 'Balanced' },
    { value: 'high', label: 'High' },
  ];
  readonly nav = [
    {
      path: 'home',
      label: 'Home',
      number: '01',
      icon: 'M3 11 12 3l9 8M6 9v12h12V9M10 21v-7h4v7',
    },
    {
      path: 'ships',
      label: 'Fleet',
      number: '02',
      icon: 'm12 2 4 9 5 8-7-3-2 6-2-6-7 3 5-8 4-9Z',
    },
    {
      path: 'atlas',
      label: 'Atlas',
      number: '03',
      icon: 'M12 3v18M3 12h18M5.6 5.6l12.8 12.8M5.6 18.4 18.4 5.6M8 12a4 4 0 1 0 8 0 4 4 0 1 0-8 0',
    },
    {
      path: 'settings',
      label: 'Settings',
      number: '04',
      icon: 'M4 6h16M4 12h16M4 18h16M8 3v6M16 9v6M10 15v6',
    },
  ];
  view: View = 'home';
  selected: Vessel = FLAGSHIP;
  preferences: Preferences;
  ready = false;
  error: string | null = null;
  help = false;
  hasSave = false;
  storageNotice = '';
  state: FlightState = {
    position: [0, 0, 0],
    sector: '0:0:0',
    speed: 0,
    altitude: 0,
    nearest: HOME_WORLD,
    distance: 867,
    planets: [HOME_WORLD],
    canDescend: false,
    surface: false,
    paused: false,
    course: null,
    navigation: null,
    saved: false,
    descending: false,
    inspecting: false,
    sortie: false,
    visitedInterior: false,
    interior: null,
    shuttleMarker: null,
    combat: null,
    practiceNotice: '',
  };
  private engine?: UniverseScene;
  private subscription: Subscription;
  private readonly diagnostics = () => this.engine?.diagnostics();

  constructor(
    private readonly router: Router,
    private readonly zone: NgZone,
    private readonly changes: ChangeDetectorRef,
  ) {
    const save = readFlightSave(this.stored('aquila.voyage.v1'));
    this.hasSave = !!save;
    this.selected = FLEET.find((v) => v.id === save?.vessel) ?? FLAGSHIP;
    this.preferences = readPreferences(
      this.stored('aquila.preferences.v1'),
      window.matchMedia('(prefers-reduced-motion: reduce)').matches,
    );
    this.subscription = router.events
      .pipe(filter((event) => event instanceof NavigationEnd))
      .subscribe(() => this.route());
  }
  private stored(key: string): string | null {
    try {
      return localStorage.getItem(key);
    } catch {
      return null;
    }
  }
  ngAfterViewInit(): void {
    this.zone.runOutsideAngular(() => {
      try {
        this.engine = new UniverseScene(
          this.host.nativeElement,
          this.preferences,
          readFlightSave(this.stored('aquila.voyage.v1')),
          (state) =>
            this.zone.run(() => {
              this.state = state;
              this.hasSave = this.hasSave || state.saved;
            }),
          (error) =>
            this.zone.run(() => {
              this.error = error;
            }),
        );
        this.zone.run(() => {
          this.ready = true;
          this.route();
          this.changes.detectChanges();
        });
        if (isDevMode())
          (window as unknown as Record<string, unknown>)['aquilaDiagnostics'] =
            this.diagnostics;
      } catch {
        this.zone.run(() => {
          this.error =
            'The universe needs WebGL 2. Try a browser with hardware acceleration enabled.';
          this.changes.detectChanges();
        });
      }
    });
  }
  private route(): void {
    const segment = this.router.url
      .split(/[?#]/)[0]
      .split('/')
      .filter(Boolean)[0];
    this.view = (
      ['home', 'ships', 'atlas', 'play', 'settings'].includes(segment)
        ? segment
        : 'home'
    ) as View;
    this.help = false;
    this.engine?.setView(this.view);
    if (this.view === 'play') this.engine?.focus();
    document.title =
      this.view === 'home'
        ? 'Aquila — Somewhere, beyond.'
        : `Aquila — ${
            this.view === 'ships'
              ? 'Fleet'
              : this.view[0].toUpperCase() + this.view.slice(1)
          }`;
  }
  selectVessel(vessel: Vessel): void {
    this.selected = vessel;
    this.engine?.setShip(vessel.id);
  }
  async launch(): Promise<void> {
    if (this.view === 'ships' && this.state.surface)
      this.engine?.returnToOrbit();
    await this.router.navigateByUrl('/play');
    this.engine?.setPaused(false);
    this.engine?.focus();
  }
  async travel(index: number): Promise<void> {
    const target = this.state.planets[index];
    if (!target) return;
    await this.router.navigateByUrl('/play');
    this.engine?.travelTo(target);
    this.engine?.focus();
  }
  descend(): void {
    this.engine?.descend();
    this.engine?.focus();
  }
  returnToOrbit(): void {
    this.engine?.returnToOrbit();
    this.engine?.focus();
  }
  async meetBattleship(): Promise<void> {
    this.selectVessel(FLAGSHIP);
    await this.router.navigateByUrl('/ships');
  }
  inspectHull(): void {
    this.engine?.inspectHull();
    this.engine?.focus();
  }
  resumeCommand(): void {
    this.engine?.resumeCommand();
    this.engine?.focus();
  }
  enterBridge(): void {
    this.engine?.enterBridge();
    this.engine?.focus();
  }
  exteriorView(): void {
    this.engine?.exteriorView();
    this.engine?.focus();
  }
  interactAboard(): void {
    this.engine?.interactAboard();
    this.engine?.focus();
  }
  returnToHangar(): void {
    this.engine?.returnToHangar();
    this.engine?.focus();
  }
  startPractice(): void {
    this.engine?.startPractice();
    this.engine?.focus();
  }
  setWeaponMode(mode: 'automatic' | 'hold'): void {
    this.engine?.setWeaponMode(mode);
    this.engine?.focus();
  }
  endPractice(): void {
    this.engine?.endPractice();
    this.engine?.focus();
  }
  setAmmunition(ammo: 'standard' | 'piercing'): void {
    this.engine?.setAmmunition(ammo);
    this.engine?.focus();
  }
  setHelmOrder(order: 'hold' | 'range' | 'withdraw'): void {
    this.engine?.setHelmOrder(order);
    this.engine?.focus();
  }
  get commanding(): boolean {
    return (
      !!this.selected.capital && !this.state.surface && !this.state.inspecting
    );
  }
  get stationAction(): string {
    const interior = this.state.interior;
    if (!interior) return '';
    if (interior.mode === 'helm') return 'Leave command console';
    if (interior.mode === 'tactical') return 'Leave tactical console';
    return interior.interaction === 'helm'
      ? 'Use command console'
      : interior.interaction === 'tactical'
      ? 'Use tactical console'
      : interior.interaction === 'shuttle'
      ? 'Board and launch Peregrine'
      : '';
  }
  togglePause(): void {
    this.engine?.setPaused(!this.state.paused);
    if (!this.state.paused) this.engine?.focus();
  }
  showHelp(): void {
    this.help = !this.help;
    if (this.view === 'play') this.engine?.setPaused(this.help);
  }
  @HostListener('window:keydown', ['$event'])
  closeHelp(event: KeyboardEvent): void {
    if (event.code === 'Escape' && this.help) this.help = false;
  }
  touch(event: PointerEvent, code: string, down: boolean): void {
    event.preventDefault();
    const target = event.currentTarget as HTMLElement;
    if (down) target.setPointerCapture(event.pointerId);
    this.engine?.input(code, down);
  }
  quality(value: Quality): void {
    if (this.preferences.quality === value) return;
    this.preferences.quality = value;
    this.savePreferences();
  }
  motion(event: Event): void {
    this.preferences.reducedMotion = (event.target as HTMLInputElement).checked;
    this.savePreferences();
  }
  sensitivity(event: Event): void {
    this.preferences.sensitivity = Number(
      (event.target as HTMLInputElement).value,
    );
    this.savePreferences();
  }
  private savePreferences(): void {
    this.engine?.setPreferences(this.preferences);
    try {
      localStorage.setItem(
        'aquila.preferences.v1',
        JSON.stringify(this.preferences),
      );
      this.storageNotice = 'Preferences saved on this device.';
    } catch {
      this.storageNotice =
        'Preferences work for this visit. Device storage is unavailable.';
    }
  }
  async returnHomeWorld(): Promise<void> {
    this.engine?.resetVoyage();
    await this.launch();
  }
  reload(): void {
    window.location.reload();
  }
  get coordinates(): string {
    return this.state.position
      .map((n) => Math.round(n).toLocaleString('en-US'))
      .join(' / ');
  }
  range(value: number): string {
    return value >= 1000
      ? `${(value / 1000).toFixed(1)} km`
      : `${Math.round(value)} m`;
  }
  planetDistance(index: number): string {
    const planet = this.state.planets[index];
    if (!planet) return '';
    return this.range(
      Math.max(
        0,
        Math.hypot(
          ...planet.position.map((n, i) => n - this.state.position[i]),
        ) - planet.radius,
      ),
    );
  }
  ngOnDestroy(): void {
    this.subscription.unsubscribe();
    this.engine?.dispose();
    const exposed = window as unknown as Record<string, unknown>;
    if (exposed['aquilaDiagnostics'] === this.diagnostics)
      delete exposed['aquilaDiagnostics'];
  }
}
