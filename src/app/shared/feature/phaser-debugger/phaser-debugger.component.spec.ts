import { ComponentFixture, TestBed } from "@angular/core/testing";
import { PhaserDebuggerComponent } from "./phaser-debugger.component";

describe("PhaserDebuggerComponent", () => {
  let component: PhaserDebuggerComponent;
  let fixture: ComponentFixture<PhaserDebuggerComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [PhaserDebuggerComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(PhaserDebuggerComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
